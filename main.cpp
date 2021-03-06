/*

Bilal Tekin
2017400264

*/


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include<sstream>
#include <algorithm>
#include<stdlib.h>
#include<math.h>
// prototypes 

//Print Bits
template <class datatype> void printBits(datatype x);
//Print Hex Values Of Parameter
template <class datatype> void print_hex(datatype x) ; 
// MovRegToReg
template <class regtype>  void mov_reg_reg(regtype *preg1,regtype *preg2)  ;
// PrintAllSixteenBixVariables
void print_16bitregs() ; 

// global variables ( memory, registers and flags ) 
unsigned char memory[2<<15] ;    // 64K memory 

// REGISTERS
unsigned short ax = 0 ; 
unsigned short bx = 0 ;
unsigned short cx = 0 ; 
unsigned short dx = 0 ;

unsigned short di = 0 ; 
unsigned short si = 0 ; 
unsigned short bp = 0 ; 

unsigned short PC = 0 ; 
// unsigned short sp = 0 ;
unsigned short sp = (2<<15)-2;


// FLAGS
bool zf = 0;              // zero flag
bool cf = 0;              // carry flag
bool af = 0;              // auxillary flag 
bool sf = 0;              // sign flag 
bool of = 0;              // overflow flag


// INSTRUCTIONS 

// JMP INSTRUCTIONS
bool jz = 0; 
bool jnz = 0; 
bool je = 0; 
bool jne = 0; 
bool ja = 0; 
bool jae = 0; 
bool jb = 0; 
bool jbe = 0; 
bool jnae = 0; 
bool jnb = 0; 
bool jnbe = 0; 
bool jnc = 0; 
bool jc = 0; 



// initialize Register pointers 
unsigned short *pax = &ax ; 
unsigned short *pbx = &bx ; 
unsigned short *pcx = &cx ; 
unsigned short *pdx = &dx ; 
unsigned short *psi = &si; 
unsigned short *pdi = &di; 
unsigned short *pbp = &bp; 
unsigned short *psp = &sp; 


// note that x86 uses little endian, that is, least significat byte is stored in lowest byte 
unsigned char *pah = (unsigned char *) ( ( (unsigned char *) &ax) + 1);
unsigned char *pal = (unsigned char *) &ax;
unsigned char *pbh = (unsigned char *) ( ( (unsigned char *) &bx) + 1);
unsigned char *pbl = (unsigned char *) &bx;
unsigned char *pch = (unsigned char *) ( ( (unsigned char *) &cx) + 1);
unsigned char *pcl = (unsigned char *) &cx;
unsigned char *pdh = (unsigned char *) ( ( (unsigned char *) &dx) + 1);
unsigned char *pdl = (unsigned char *) &dx;

using namespace std;

//   OBJECTS

// Object For Db Variables
struct dbVariable{
    string name;
    string type = "db";
    int address = 0;
    unsigned char valueOld = 0;
    unsigned char *value;
};

// Object For Dw Variables
struct dwVariable{
    string name;
    string type = "dw";
    int address = 0;
    unsigned short valueOld = 0;
    unsigned short *value;
};

// Object For Labels
struct Label{
    string name = "";
    vector<string> content;
};


//   CONSTANT
string fromKeyboard = "";
string temp,firstWord,secondWord,thirdWord,forthWord,fifthWord,sixthWord,seventhWord = "";
vector<dwVariable> dwVariables;
vector<dbVariable> dbVariables;
vector<Label> labels;
bool isint20h = false;
bool isJustAfterCompare = false;
bool lineWithoutLabel = false;
int numberOfLinesWithoutVariables = 0;
bool canJump = false;
vector<pair<string,string>> queueOfVariables;


// FUNCTIONS

// **********************************************
//    C H E C K S

// CheckingSpaces
bool checkSpace(string& line);
//CheckingQuotationMarks
bool checkQuotationMarks(string& line);
//CheckingSingleQuotationMarks
bool checkSingleQuotationMark(string& line);
//CheckingComma
bool checkComma(string& line);
//CheckingSemiColon
bool checkSemiColon(string& line);
//Checking int20h
bool checkint20h(string& line);
//CheckingForDecimalDigtis
bool isDigitDecimal(string variable,int digit);
//CheckingBrakets
bool checkBrackets(string& line);
//CheckForJumpConditions
bool checkForJumpCondition(string jmpType);
//CheckingForByteValues
bool checkForB(string parameter);
//CheckingForWordValues
bool checkForW(string parameter);

// GET VALUE OF SOMETHING
//DetermineValueOfInstructions
int determineValueOfInstruction(string reg);
//GetOtherValues
int getOtherValue(string str1);
//GetAddressOfVariables
int getVariableAddress(string& variable);
//GetVariableValueFromMemoryAddress
int getVariableValueFromMemoryAddress(string address);
//GetVariableNameFromVariableAddress
string getVariableNameFromVariableAddress(string address);
//GetIndexOfLabel
int getIndexOfLabel(string & labelName);
//DetermineRegisterAndVariables
void determineReg(unsigned short **pmx, unsigned char **pmhl, string& reg,bool& isVariableFound1,bool& isVariableFound2,std::vector<dbVariable>::iterator &it,std::vector<dwVariable>::iterator &it2);


// SET VALUE OF SOMETHING
//SetAddressOfVariables
void setVariableAddress(string& variable,int address);
//SetVariableValues
void setVariableValue(string variableName,int value);
//CheckAndSetFlags
void checkAndSetFlags(int number1,int number2,int bit,string option);


// CLEANING AND PARSING

//CleanVariablesFrom [ , h , d , b , ]
string cleanVariable(string variable);
//TrimSpaces
inline std::string trim(std::string& str);
//GetNextLineInTheInputFileAndTrimItAndLowerIt
void getLineTrimLower(ifstream& inFile,string& firstLine);
//GetWordsInsideOfLines
inline string getLinestreamLine(istringstream& linestream,string& word,char option);
//Parse Line such as mov ax,bx
void twoWordsComma(istringstream& linestream,string& secondWord, string& thirdWord);
//Parse Line such as mov ax, offset var
void thirdWordsComma(istringstream& linestream,string& secondWord, string& thirdWord, string& forthWord);

// Parse Input
void parseInput(string& line,ifstream& inFile);
//Create Labels
void createLabel(ifstream& inFile,string& labelName);
//CreateInstructionsWhichDoes not have label Name
void createLinesWithoutLabels(ifstream& inFile,string& stringName);
//Create Db or Dw variables
void createDbOrDw(string& line);
// Get Content Of Labels: Instuctions inside labels
void getLabelContent(Label& label,ifstream& inFile,string& line);
//Determine Variables inside labels
void determineLabelVariables();
//put value of variables inside memory
void constructMemory();
//Process label instructions
void processLabels(int index);
//ProcessTwoWordInstructions such as mov ax,bx
void processTwoWordsInstructions(string& option, string& str1,string& str2,string& str3);
//ProcessOneWordInstructions such as mov ax, offset var
void processOneWordInstructions(string& option, string& str1);
//Converting Instuctions such as [bx] into [01100b], [sp] to [000303h]
void strToMemoryAddress(string& str1,string& str2);
//CheckWhetherItIsASixteenBitValue
bool isItSixteenBitValue(string str1);



//UTILITY FUNCTIONS

// Convert Hexadecimal To Decimal
unsigned short hexToDec(std::string hexString);
// Convert Decimal To Hexadecimal
string decToHex(int n);
// Convert Decimal To Binary
template <class datatype> string decToBin(datatype x);
// Convert Binary To Decimal
int binToDec(string number);
// Convert Binary To Decimal
int binToDec(long long int n);
//MakeLineUpperCase
void toUpper(string& firstLine);
//MakeLineLowerCase
void toLower(string& firstLine);
//Count Specific Characters In the word
int countSpecificCharacter(string str, char character);
//Print Labels
void printLabels();
//Print Variables
void printVariables();
//Get Right 8 bit of the number
int getRightEightBitValueOfNumber(int number);
// Get Left Bits bitween right 16-8 of the number 
int getLeftEightBitValueOfNumber(int number);
//Get Sixteen Bit value of the number
int getSixteenBitValueOfNumber(int number);



// INSTRUCTIONS
// Process Instructions such as mov
void instructionOptions(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,std::vector<dbVariable>::iterator firstIt,std::vector<dwVariable>::iterator firstIt2,bool& isFirstVariableFound1,bool& isFirstVariableFound2,string& str1,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl,string option);
// Move Value to variable
template <class regOne, class regTwo>  void moveValueToVariable(regOne& firstIt,regTwo *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,string& str1,string& str2,string& str3,string option);
//Move value to register
template <class regOne, class regTwo>  void moveValueToReg(regOne** firstReg, regTwo* secondReg,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,string& str2,string& str3,string option);
//Process lines with Brakets
void instructionForBrakets(string str1,string str2,string str3,string option);
//Add Instructions
void add(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl);
//Sub Instructions
void sub(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl);
//CMP Instructions
void comparison(unsigned short firstValue,unsigned short secondValue);
//Update Value inside memory
template <class typeOfVariableValue> void setMemoryForDbAndDw(int address,typeOfVariableValue variableValue,string typeOfVariable);
//If There is an error exit from program
void exitFromExecution(string paramaterError);
//Checking For Errors
void checkForCompatibility(string option,string str1,string str2);
//Checking if it is a one word instruction, for example, push bx
bool isItOneWordInstruction(string a);
//Checking errors in the line
void isValidLine(string a,string b,string c);
//Checking for invalid instructions such as 'topla'
bool isValidInstruction(string a);
//Checking for invalid registers
bool isValidRegister(string a);
//Checking for invalid variable names
bool isValidVariable(string a);
//CheckingForOtherValues Such as 10,10d,10h,'a',[0110] etc.
bool isValidOtherValue(string str1);
//Checking invalid variable names
void isValidVariableName(string variableName);
//checking Invalid Variable Names
void checkForInvalidVariableNames();
//Checking invalid label names
void checkForInvalidLabelNames();

//Main Functions
int main(int argc, char* argv[]) {

    // Open the input and output files, check for failures
    ifstream inFile(argv[1]);
    if (!inFile) { // operator! is synonymous to .fail(), checking if there was a failure
        cerr << "There was a problem when opening " << argv[1] << " as an input file" << endl;
        return 1;
    }

   string firstLine;
   getLineTrimLower(inFile,firstLine);

   // This While for parsing input
   while(!checkint20h(firstLine) && !inFile.eof()){
      parseInput(firstLine,inFile);  
      getLineTrimLower(inFile,firstLine);
   }
   determineLabelVariables();
   constructMemory();
   isint20h = false;
   checkForInvalidVariableNames();
   checkForInvalidLabelNames();
   processLabels(0);
}


// INPUT PARSING
void parseInput(string& line,ifstream& inFile){
   if(line.find("code segment") != string::npos || line.find("code ends") != string::npos) return;
   bool isDb = line.find("db") != string::npos && line.find(",") == string::npos;
   bool isDw = line.find("dw") != string::npos && line.find(",") == string::npos;
   if(line == "") return;
   if(isDb || isDw){
      createDbOrDw(line);
      
   }else if(checkSemiColon(line)){
      createLabel(inFile,line);
   }else{
      createLinesWithoutLabels(inFile,line);
   }


}


// CREATE LABEL
void createLabel(ifstream& inFile,string& stringName){
   string labelName = "";
   istringstream linestream(stringName);
   getline(linestream,labelName,':');
   Label label;
   label.name = trim(labelName);
   getLabelContent(label,inFile,stringName);
   
}

// CREATE LABEL WITHOUT NAMES
void createLinesWithoutLabels(ifstream& inFile,string& stringName){
   string labelName = "";
   Label label;
   label.name = "WithoutLabels";
   lineWithoutLabel = true;
   getLabelContent(label,inFile,stringName);
}

// VARIABLES
void createDbOrDw(string& line){
   istringstream linestream(line);
   string temp,firstWord,secondWord,thirdWord,forthWord,fifthWord,sixthWord,seventhWord = "";
   if(checkSpace(line)){
      getLinestreamLine(linestream,firstWord,' ');
      getLinestreamLine(linestream,secondWord,' ');
      getLinestreamLine(linestream,thirdWord,' ');
      getLinestreamLine(linestream,forthWord,' ');
      getLinestreamLine(linestream,fifthWord,' ');
      // cout << "FirstWord : " << firstWord << " - Second Word : " << secondWord << " - thirdWord : " << thirdWord << " - ForthWord : " << forthWord << " - FIfth Word : " << fifthWord << endl;
      if(isValidInstruction(thirdWord) || isValidRegister(thirdWord)) exitFromExecution("Invalid Variable Value !!! " + thirdWord);

      if(secondWord == "db"){
         dbVariable variable;
         variable.name = firstWord;
         if(checkQuotationMarks(thirdWord) || checkSingleQuotationMark(thirdWord)){
            if((unsigned short)thirdWord.at(1) > 255) {
               exitFromExecution("Variable Value is too large to fit into db memory ");
            }
            variable.valueOld = (unsigned char)thirdWord.at(1);
         }else{
            if(determineValueOfInstruction(thirdWord) > 255) {
               exitFromExecution("Variable Value is too large to fit into db memory ");
            }
            variable.valueOld = (unsigned char) determineValueOfInstruction(thirdWord);
         }
         dbVariables.push_back(variable);
         queueOfVariables.push_back(make_pair(variable.name,"db"));
      }else{
         dwVariable variable;
         variable.name = firstWord;
         if(checkQuotationMarks(thirdWord) || checkSingleQuotationMark(thirdWord)){
            if((unsigned short)thirdWord.at(1) > 65535) {
               exitFromExecution("Variable Value is too large to fit into dw memory ");
            }
            variable.valueOld = (unsigned short)thirdWord.at(1);
         }else{
         if(determineValueOfInstruction(thirdWord) > 65535) {
               exitFromExecution("Variable Value is too large to fit into dw memory ");
            }
            variable.valueOld = (unsigned short)determineValueOfInstruction(thirdWord);
         
         }
         
         dwVariables.push_back(variable);
         queueOfVariables.push_back(make_pair(variable.name,"dw"));

      }
      
      
   }
}

// GET LABEL CONTENT
void getLabelContent(Label& label,ifstream& inFile,string& line){
   string firstLine = line;
   if(!lineWithoutLabel) getLineTrimLower(inFile,firstLine);
   while(!checkSemiColon(firstLine)){
      if(firstLine.find("code segment") != string::npos || firstLine.find("code ends") != string::npos) break;
      if((firstLine.find("db") != string::npos || firstLine.find("dw") != string::npos)) {
         if(firstLine.substr(0,2) == "db" || firstLine.substr(0,2) == "dw"){
            string temp = label.name + " " + firstLine;
            createDbOrDw(temp);
         }else{
            createDbOrDw(firstLine);
         }
         if(inFile.eof()) break;
         getLineTrimLower(inFile,firstLine);
         continue;
      }
      if(firstLine.find("int 20h") != string::npos) isint20h = true;
      if(firstLine != "\r" && firstLine != "") label.content.push_back(firstLine);
      if(inFile.eof()) break;
      getLineTrimLower(inFile,firstLine);

   }
   labels.push_back(label);
   lineWithoutLabel = false;
   if(checkSemiColon(firstLine)){
      createLabel(inFile,firstLine);
   }
}

// Determine Label Variables
void determineLabelVariables(){
   string line = "";
   std::vector<string>::iterator it;
   for (int i = 0; i < labels.size(); i++) {
      numberOfLinesWithoutVariables += labels.at(i).content.size();  
   for (it = labels.at(i).content.begin(); it != labels.at(i).content.end(); it++) {
      line = *it;
      if(line.find("int 20h") != string::npos) return;
      }
   }
}



// CONSTRUCT MEMORY
void constructMemory(){
   vector<pair<string,string>>::iterator it;
   int address = 0;
   int whereAddressLeft = numberOfLinesWithoutVariables * 6;
   for (it = queueOfVariables.begin(); it != queueOfVariables.end(); it++) {
      setVariableAddress((*it).first,whereAddressLeft);
      if((*it).second == "dw") whereAddressLeft +=2;
      else if((*it).second == "db") whereAddressLeft +=1;
   }
   
}



// SET VARIABLE ADDRESS
void setVariableAddress(string& variable,int address){
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      for (it = dbVariables.begin(); it != dbVariables.end(); it++) {
         if(variable == (*it).name){
            (*it).address = address;
            (*it).value = (unsigned char *) &memory[address];
            (*(*it).value) = (*it).valueOld;
            isVariableFound1 = true;
            break;
         }

      }
      if(!isVariableFound1){
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
      {
         if(variable == (*it2).name){
            (*it2).address = address;
            (*it2).value = (unsigned short *) &memory[address];
            (*(*it2).value) = (*it2).valueOld;
            isVariableFound2 = true;
            break;
         }
      }
      }
}

// PROCESS LABEL
void processLabels(int index){
   int index2 = -1;
   string line = "";
   std::vector<string>::iterator it;
   canJump = false;
   string newLabel = "";
   for (int i = index; i < labels.size() && !canJump; i++) {
   for (it = labels.at(i).content.begin(); it != labels.at(i).content.end() && !canJump; it++) {
   string temp,firstWord,secondWord,thirdWord,forthWord,fifthWord,sixthWord,seventhWord = "";

      line = (*it);
      istringstream linestream(line);
      getLinestreamLine(linestream,firstWord,' ');
      // Bu Satiri Biraz Daha Gelistir
       if(!isValidInstruction(firstWord)) exitFromExecution("ERROR : Invalid Instruction : " + firstWord);
      // Bu Satiri Sonra Sil
      if(firstWord == "mov" || firstWord == "add" || firstWord == "sub" || firstWord == "xor" || firstWord == "or" || firstWord == "and" || firstWord == "shr" || firstWord == "shl" || firstWord == "rcl" || firstWord == "rcr" || firstWord == "cmp"){
         if(line.find("offset") != string::npos) thirdWordsComma(linestream,secondWord,thirdWord,forthWord);
         else twoWordsComma(linestream,secondWord,thirdWord);
         strToMemoryAddress(secondWord,thirdWord);
         isValidLine(firstWord,secondWord,thirdWord);
         if(forthWord != "offset") checkForCompatibility(firstWord,secondWord,thirdWord);
         processTwoWordsInstructions(firstWord,secondWord,thirdWord,forthWord);
      }else if(firstWord == "int" || firstWord == "mul" || firstWord == "div" || firstWord == "push" || firstWord == "pop" || firstWord == "inc" || firstWord == "dec" || firstWord == "not"){
         getLinestreamLine(linestream,secondWord,' ');
         strToMemoryAddress(secondWord,thirdWord);
         isValidLine(firstWord,secondWord,thirdWord);
         checkForCompatibility(firstWord,secondWord,thirdWord);
         processOneWordInstructions(firstWord,secondWord);
      }else if(firstWord == "nop"){
         
      }else if(firstWord.at(0) == 'j'){
         isValidLine(firstWord,secondWord,thirdWord);
         getLinestreamLine(linestream,secondWord,' ');
         if(secondWord.at(secondWord.length()-1) == '\r') secondWord = secondWord.substr(0,secondWord.length()-1);

         if(firstWord == "jmp"){
            index2 = getIndexOfLabel(secondWord);
            if(index2 == -1) exitFromExecution("THERE IS NOT ANY LABEL WITH THIS NAME : " + secondWord);
            canJump = true;
         }else{
            canJump = checkForJumpCondition(firstWord);
            if(canJump){
               index2 = getIndexOfLabel(secondWord);
               if(index2 == -1) exitFromExecution("THERE IS NOT ANY LABEL WITH THIS NAME : " + secondWord);
            }

         }
         
      }
      // print_16bitregs();
      // cout << "AX: " << endl; 
      // printBits(ax);
      // cout << "BX: " << endl;
      // printBits(bx);
      // cout << "CX: "  << endl;
      // printBits(cx);
      // cout << "DX: " << endl;
      // printBits(dx);

   }

   }

   if(index2 != -1) processLabels(index2);
}

// Cheking if it is fine to jump, this method will return true;
bool checkForJumpCondition(string jmpType) {
   if(jmpType == "jz" || jmpType == "je"){
      if(zf) return true;
   }else if(jmpType == "jnz" || jmpType == "jne"){
      if(!zf) return true;
   }else if(jmpType == "ja" || jmpType == "jnbe"){
      if(!cf && !zf) return true;
   }else if(jmpType == "jae" || jmpType == "jnc" || jmpType == "jnb"){
      if(!cf) return true;
   }else if(jmpType == "jb" || jmpType == "jnae" || jmpType == "jc"){
      if(cf) return true;
   }else if(jmpType == "jbe" || jmpType == "jna"){
      if(cf || zf) return true;
   }
   return false;
}
// PROCESS TWO WORDS INSTRUCTIONS, for example, mov ax,10 or add ax,bx
void processTwoWordsInstructions(string& option, string& str1,string& str2,string& str3){
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      std::vector<dbVariable>::iterator firstIt;
      std::vector<dwVariable>::iterator firstIt2;
      unsigned char *pmhl = nullptr;
      unsigned char *pnhl = nullptr;
      unsigned short *pmx = nullptr; 
      unsigned short *pnx = nullptr;   
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      bool isFirstVariableFound1 = false;
      bool isFirstVariableFound2 = false;


      determineReg(&pmx,&pmhl,str1,isFirstVariableFound1,isFirstVariableFound2,firstIt,firstIt2);
      if(str3 != "offset")
      determineReg(&pnx,&pnhl,str2,isVariableFound1,isVariableFound2,it,it2);


      if(option == "mov" || option == "add" || option == "sub" || option == "or" || option == "and" || option == "xor" || option == "not" || option == "rcl" || option == "rcr" || option == "shr" || option == "shl" || option == "shl" || option == "cmp"){
         instructionOptions(pmx,pnx,it,it2,firstIt,firstIt2,isFirstVariableFound1,isFirstVariableFound2,str1,str2,str3,pmhl,pnhl,option);
      }

}

// PROCESS ONE WORD INSTRUCTIONS. For example, push ax, pop bx, mul w 10
void processOneWordInstructions(string& option, string& str1){
   if(option == "int" && str1 == "21h"){
      if(*pah == 1){
         cin.clear();
         string s = "";
         getline(cin,s);
         unsigned char temp = s.at(0);
         *pal = temp;
      }else if(decToHex(*pah) == "2"){
         cout << (char)(*pdl) ;
         *pal = *pdl;
      }else if(decToHex(*pah) == "8"){
         char temp;
         cin >> temp;

         *pal = (unsigned char)temp;
      }else if(decToHex(*pah) == "9"){
         if(*pdx == 255){
            cout << fromKeyboard;
         }else{
            cout << (char)(*pdh);
            cout << (char)(*pdl);
         }
      }else if(decToHex(*pah) == "A"){
         cin >> fromKeyboard;
         *pdx = 255;
      }else if(decToHex(*pah) == "4C"){
         exit(*pal);
      }
      cout.flush();
   }else if(option == "int" && str1 == "20h"){
      // print_16bitregs();
      exit(0);
   }else if(option == "div"){
      if(determineValueOfInstruction(str1) == 0) exitFromExecution("ERROR : You Can not Divide With Zero");
      unsigned short willDivide = (unsigned short)determineValueOfInstruction(str1);
      if(isItSixteenBitValue(str1)){
         int resultOfDxAndAx = ((*pdx) << 16) + (*pax);
         checkAndSetFlags(resultOfDxAndAx,willDivide,16,option);
         unsigned short quotient = resultOfDxAndAx / willDivide;
         unsigned short remainder = resultOfDxAndAx % willDivide;
         *pdx = remainder;
         *pax = quotient;
      }else{
         int resultOfDxAndAx = (*pax);
         checkAndSetFlags(resultOfDxAndAx,willDivide,8,option);
         unsigned char quotient = *pax / willDivide;
         unsigned char remainder = *pax % willDivide;
         *pah = remainder;
         *pal = quotient;
      }
   }else if(option == "mul"){
      if(isItSixteenBitValue(str1)){
         int result = *pax * (unsigned short)determineValueOfInstruction(str1);
         if(binToDec(decToBin(result).substr(0,16)) != 0) cf = 1;
         else cf = 0;
         *pdx = binToDec(decToBin(result).substr(0,16));
         *pax = binToDec(decToBin(result).substr(16,32));

      }else{
         unsigned short result = *pal * (unsigned short)determineValueOfInstruction(str1);
         if(binToDec(decToBin(result).substr(0,8)) != 0) cf = 1;
         else cf = 0;
         *pax = *pal * determineValueOfInstruction(str1);

      }
      // setEightAndSixteenBitBoolValues(pmx,pnx,pmhl,pnhl,str1,str2);
   }else if(option == "inc"){
      unsigned short *pmx = nullptr; 
      unsigned char *pmhl = nullptr;
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      determineReg(&pmx,&pmhl,str1,isVariableFound1,isVariableFound2,it,it2);
      if(pmx != nullptr) {
         checkAndSetFlags(*pmx,1,16,option);
         *pmx += 1;
      }
      else if(pmhl != nullptr) {
         checkAndSetFlags(*pmhl,1,8,option);
         *pmhl += 1;
      }                                                                          
      else if(isVariableFound1) {
         string type = "";
         int bit = 0;
         unsigned short result = 0;
         if(isItSixteenBitValue(str1)) result = memory[(*it).address] + memory[(*it).address + 1] * pow(2,8);
         else result = memory[(*it).address] ;
         
         bit = isItSixteenBitValue(str1) ? 16 : 8; 
         type = bit == 16 ? "dw" : "db";
         checkAndSetFlags(result,1,bit,option);
         setMemoryForDbAndDw((*it).address,++result,type);


      }
      else if(isVariableFound2) {

         string type = "";
         int bit = 0;
         unsigned short result = 0;
         if(isItSixteenBitValue(str1)) result = memory[(*it2).address] + memory[(*it2).address + 1] * pow(2,8);
         else result = memory[(*it2).address] ;
         
         bit = isItSixteenBitValue(str1) ? 16 : 8; 
         type = bit == 16 ? "dw" : "db";
         checkAndSetFlags(result,1,bit,option);
         setMemoryForDbAndDw((*it2).address,++result,type);


      }else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(str1,str1.find_first_of('[')+1)){
         int result = 0;
         int address = stoi(cleanVariable(str1));
         int bit = isItSixteenBitValue(str1) ? 16 : 8;
         string type = bit == 16 ? "dw" : "db";
         result = (type == "dw") ? memory[address] + memory[address+1] * pow(2,8) + 1 : memory[address] + 1;
         checkAndSetFlags(result-1,1,bit,option);
         setMemoryForDbAndDw(address,(unsigned short)result,type);
      }
   }else if(option == "dec"){
      unsigned short *pmx = nullptr; 
      unsigned char *pmhl = nullptr;
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      determineReg(&pmx,&pmhl,str1,isVariableFound1,isVariableFound2,it,it2);
      if(pmx != nullptr){
         checkAndSetFlags(*pmx,1,16,option);
         *pmx-=1;
      } 
      else if(pmhl != nullptr) {
         checkAndSetFlags(*pmhl,1,8,option);
         *pmhl-=1;
      }                                                                          
      else if(isVariableFound1) {

         string type = "";
         int bit = 0;
         unsigned short result = 0;
         if(isItSixteenBitValue(str1)) result = memory[(*it).address] + memory[(*it).address + 1] * pow(2,8);
         else result = memory[(*it).address] ;
         
         bit = isItSixteenBitValue(str1) ? 16 : 8; 
         type = bit == 16 ? "dw" : "db";
         checkAndSetFlags(result,1,bit,option);
         setMemoryForDbAndDw((*it).address,--result,type);

      }
      else if(isVariableFound2) {

         string type = "";
         int bit = 0;
         unsigned short result = 0;
         if(isItSixteenBitValue(str1)) result = memory[(*it2).address] + memory[(*it2).address + 1] * pow(2,8);
         else result = memory[(*it2).address] ;
         
         bit = isItSixteenBitValue(str1) ? 16 : 8; 
         type = bit == 16 ? "dw" : "db";
         checkAndSetFlags(result,1,bit,option);
         setMemoryForDbAndDw((*it2).address,--result,type);


      }else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(str1,str1.find_first_of('[')+1)){
         int result = 0;
         int address = stoi(cleanVariable(str1));
         string type = isItSixteenBitValue(str1) ? "dw" : "db";
         int bit = isItSixteenBitValue(str1) ? 16 : 8;
         result = (type == "dw") ? memory[address] + memory[address+1] * pow(2,8) - 1 : memory[address] - 1;
         checkAndSetFlags(result+1,1,bit,option);
         setMemoryForDbAndDw(address,(unsigned short)result,type);
      }
   }else if(option == "not"){
      unsigned short *pmx = nullptr; 
      unsigned char *pmhl = nullptr;
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      determineReg(&pmx,&pmhl,str1,isVariableFound1,isVariableFound2,it,it2);
      if(pmx != nullptr){
         *pmx = ~(*pmx);
      } 
      else if(pmhl != nullptr) {
         *pmhl = ~(*pmhl);
      }                                                                          
      else if(isVariableFound1) {

         string type = "";
         int bit = 0;
         if(isItSixteenBitValue(str1)) {
            unsigned short result = 0;
            result = memory[(*it).address] + memory[(*it).address + 1] * pow(2,8);
            bit = 16;
            type = "dw";
            result = ~result;
            setMemoryForDbAndDw((*it).address,result,type);
         }
         else {
            unsigned char result = 0;
            result = memory[(*it).address] ;
            bit = 8; 
            type = "db";
            result = ~result;
            setMemoryForDbAndDw((*it).address,result,type);
         }
         
         

      }
      else if(isVariableFound2) {

         string type = "";
         int bit = 0;
         if(isItSixteenBitValue(str1)) {
            unsigned short result = 0;
            result = memory[(*it2).address] + memory[(*it2).address + 1] * pow(2,8);
            bit = 16;
            type = "dw";
            result = ~result;
            setMemoryForDbAndDw((*it2).address,result,type);
         }
         else {
            unsigned char result = 0;
            result = memory[(*it2).address] ;
            bit = 8; 
            type = "db";
            result = ~result;
            setMemoryForDbAndDw((*it2).address,result,type);
         }


      }else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(str1,str1.find_first_of('[')+1)){
         int address = stoi(cleanVariable(str1));
         string type = "dw";
         int bit = 16;
         if(isItSixteenBitValue(str1)) {
            unsigned short result = 0;
            string type = "dw";
            int bit = 16;
            result = memory[address] + memory[address+1] * pow(2,8);
            result = ~result;
            setMemoryForDbAndDw(address,result,type);
         }
         else {
            unsigned char result = 0;
            string type = "db";
            int bit = 8;
            result = memory[address];
            result = ~result;
            setMemoryForDbAndDw(address,result,type);
         }
      }
   }else if(option == "push"){
      unsigned short deger = (unsigned short)determineValueOfInstruction(str1);
      memory[sp] = binToDec(decToBin(deger).substr(8,16));
      memory[sp+1] = binToDec(decToBin(deger).substr(0,8));
      sp -= 2;
      // print_16bitregs() ; 
   }else if(option == "pop"){
      // Determine Reg te [0090h] gibi sayilarida belirle, pop [0090h] diyebilir.
      if(sp >= 65534) exitFromExecution("ERROR : There is no any number to pop");
      sp += 2;
      unsigned short *pmx = nullptr; 
      unsigned char *pmhl = nullptr;
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      determineReg(&pmx,&pmhl,str1,isVariableFound1,isVariableFound2,it,it2);
      if(pmx != nullptr) *pmx = memory[sp]+memory[sp+1]*pow(2,8);
      else if(pmhl != nullptr) *pmhl = memory[sp];                                                                          
      else if(isVariableFound1) {
         if(isItSixteenBitValue(str1)){ memory[(*it).address] = memory[sp]; memory[(*it).address + 1] = memory[sp + 1]; }
         else memory[(*it).address] = memory[sp];
      }
      else if(isVariableFound2) {
         if(isItSixteenBitValue(str1)){ memory[(*it2).address] = memory[sp]; memory[(*it2).address + 1] = memory[sp + 1]; }
         else memory[(*it).address] = memory[sp];
      }else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(str1,str1.find_first_of('[')+1)){
         int address = stoi(cleanVariable(str1));
         if(isItSixteenBitValue(str1)){
            memory[address] = memory[sp];
            memory[address + 1] = memory[sp + 1];
         }
         else memory[address] = memory[sp];
      }
      memory[sp] = 0;
      memory[sp+1] = 0;
      // print_16bitregs() ; 

      
   }

}


//This method converts strings like [bx],[si] to w,b [address]: and [address] to w,b [address] according to other parameter;
void strToMemoryAddress(string& str1,string& str2) {
   string temp = str1;
   string temp2 = str2;
   bool isFirstFound = false;
   bool isSecondFound = false;

   if(temp.find('[') != string::npos && temp.find(']') != string::npos){
      isFirstFound = true;
      temp = temp.substr(temp.find_first_of('[')+1,temp.length());
      temp = temp.substr(0,temp.find_last_of(']'));
   }

   if(temp2.find('[') != string::npos && temp2.find(']') != string::npos){
      isSecondFound = true;
      temp2 = temp2.substr(temp2.find_first_of('[')+1,temp2.length());
      temp2 = temp2.substr(0,temp2.find_last_of(']'));
   }

   trim(temp);
   trim(temp2);



   if(isFirstFound && (temp == "bx" || temp == "si" || temp == "di" || temp == "bp")){
      std::stringstream sstm;
      if(str1.at(0) == 'w' && temp == "bx")
         sstm << "w[" << (*pbx) << "d]";
      else if(str1.at(0) == 'b' && temp == "bx")
         sstm << "b[" << (*pbx) << "d]";
      else if(temp == "bx" && isItSixteenBitValue(str2))
         sstm << "w[" << (*pbx) << "d]";
      else if(temp == "bx" && !isItSixteenBitValue(str2))
         sstm << "b[" << (*pbx) << "d]";

      else if(str1.at(0) == 'w' && temp == "si")
         sstm << "w[" << (*psi) << "d]";
      else if(str1.at(0) == 'b' && temp == "si")
         sstm << "b[" << (*psi) << "d]";
      else if(temp == "si" && isItSixteenBitValue(str2))
         sstm << "w[" << (*psi) << "d]";
      else if(temp == "si" && !isItSixteenBitValue(str2))
         sstm << "b[" << (*psi) << "d]";
      
      else if(str1.at(0) == 'w' && temp == "di")
         sstm << "w[" << (*pdi) << "d]";
      else if(str1.at(0) == 'b' && temp == "di")
         sstm << "b[" << (*pdi) << "d]";
      else if(temp == "di" && isItSixteenBitValue(str2))
         sstm << "w[" << (*pdi) << "d]";
      else if(temp == "di" && !isItSixteenBitValue(str2))
         sstm << "b[" << (*pdi) << "d]";

      else if(str1.at(0) == 'w' && temp == "bp")
         sstm << "w[" << (*pbp) << "d]";
      else if(str1.at(0) == 'b' && temp == "bp")
         sstm << "b[" << (*pbp) << "d]";
      else if(temp == "bp" && isItSixteenBitValue(str2))
         sstm << "w[" << (*pbp) << "d]";
      else if(temp == "bp" && !isItSixteenBitValue(str2))
         sstm << "b[" << (*pbp) << "d]";   

      str1 = sstm.str();
      
   }else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(temp,0)){
       std::stringstream sstm1;
       
       try{
         char type = 'd';
         if(temp.at(temp.length() -1 ) == 'd' || temp.at(temp.length() -1 ) == 'h' || temp.at(temp.length() -1 ) == 'b'){
            type = temp.at(temp.length()-1);
            temp = temp.substr(0,temp.length()-1);
         }
         if(str1.at(0) != 'w' && str1.at(0) != 'b'){
            stoi(temp);
            if(isItSixteenBitValue(str2))
               sstm1 << "w[" << temp << type << ']';          
            else
               sstm1 << "b[" << temp << type << ']';          
         }else{
            stoi(temp);
            sstm1 << str1.at(0) << '[' << temp << type << ']';
         }
         str1 = sstm1.str();
       }catch(exception e){
          exitFromExecution("ERROR : INVALID MEMORY LOCATION !!!");
       }
    }



   if(isSecondFound && (temp2 == "bx" || temp2 == "si" || temp2 == "di" || temp2 == "bp")){
      std::stringstream sstm2;
      if(str2.at(0) == 'w' && temp2 == "bx")
         sstm2 << "w[" << (*pbx) << "d]";
      else if(str2.at(0) == 'b' && temp2 == "bx")
         sstm2 << "b[" << (*pbx) << "d]";
      else if(temp2 == "bx" && isItSixteenBitValue(str1))
         sstm2 << "w[" << (*pbx) << "d]";
      else if(temp2 == "bx" && !isItSixteenBitValue(str1))
         sstm2 << "b[" << (*pbx) << "d]";

      else if(str2.at(0) == 'w' && temp2 == "si")
         sstm2 << "w[" << (*psi) << "d]";
      else if(str2.at(0) == 'b' && temp2 == "si")
         sstm2 << "b[" << (*psi) << "d]";
      else if(temp2 =="si" && isItSixteenBitValue(str1))
         sstm2 << "w[" << (*psi) << "d]";
      else if(temp2 == "si" && !isItSixteenBitValue(str1))
         sstm2 << "b[" << (*psi) << "d]";
      
      else if(str2.at(0) == 'w' && temp2 == "di")
         sstm2 << "w[" << (*pdi) << "d]";
      else if(str2.at(0) == 'b' && temp2 == "di")
         sstm2 << "b[" << (*pdi) << "d]";
      else if(temp2 == "di" && isItSixteenBitValue(str1))
         sstm2 << "w[" << (*pdi) << "d]";
      else if(temp2 == "di" && !isItSixteenBitValue(str1))
         sstm2 << "b[" << (*pdi) << "d]";

      else if(str2.at(0) == 'w' && temp2 == "bp")
         sstm2 << "w[" << (*pbp) << "d]";
      else if(str2.at(0) == 'b' && temp2 == "bp")
         sstm2 << "b[" << (*pbp) << "d]";
      else if(temp2 == "bp" && isItSixteenBitValue(str1))
         sstm2 << "w[" << (*pbp) << "d]";
      else if(temp2 == "bp" && !isItSixteenBitValue(str1))
         sstm2 << "b[" << (*pbp) << "d]";   

      str2 = sstm2.str();
   }else if(str2.find('[') != string::npos && str2.find(']') != string::npos && isDigitDecimal(temp2,0)){
       std::stringstream sstm2;
       try{
         char type = 'd';
         if(temp2.at(temp2.length() -1) == 'd' || temp2.at(temp2.length() -1) == 'h' || temp2.at(temp2.length() -1) == 'b'){
            type = temp2.at(temp2.length()-1);
            temp2 = temp2.substr(0,temp2.length()-1);
         }
         if(str2.at(0) != 'w' && str2.at(0) != 'b'){
            stoi(temp2);
            if(isItSixteenBitValue(str1))
               sstm2 << "w[" << temp2 << type << "]";       
            else
               sstm2 << "b[" << temp2 << type << "]";       
         }else{
            stoi(temp2);
            sstm2 << str2.at(0) <<  "[" << temp2 << type << "]";       
         }
         str2 = sstm2.str();
       }catch(exception e){
          exitFromExecution("ERROR : INVALID MEMORY LOCATION !!!");
       }
    }

}

//checking for invalid variables
void checkForInvalidVariableNames() {
      std::vector<pair<string,string>>::iterator it;
      for (it = queueOfVariables.begin(); it != queueOfVariables.end(); it++) {
         isValidVariableName((*it).first);
      }
}

//Checking Invalid Label Names
void checkForInvalidLabelNames() {
      std::vector<Label>::iterator it;
      
      for (it = labels.begin(); it != labels.end(); it++) {
         if(((*it).name).find(' ') != string::npos || (*it).name == "label") exitFromExecution("ERROR : Invalid Label Name !!! : " + (*it).name);
      }
}

//Check whether variable name is valid or not
void isValidVariableName(string variableName) {
   toLower(variableName);
   if(isValidInstruction(variableName) || isValidRegister(variableName)) exitFromExecution("ERROR : " + variableName + " IS NOT A VALID VARIABLE NAME !!!");
   else if(variableName == "label" || variableName == "db" || variableName == "dw" || variableName == "w" || variableName == "b") exitFromExecution("ERROR : " + variableName + " IS NOT A VALID VARIABLE NAME !!!");
}

// Checking whole line if it is correct or not
void isValidLine(string a,string b,string c) {
   if(a != "" && !isValidInstruction(a)) exitFromExecution("INVALID INSTRUCTION : " + a);
   else if(b != "" && !isValidRegister(b) && !isValidVariable(b) && !isValidOtherValue(b)) exitFromExecution("INVALID INSTRUCTION : " + b);
   else if(c != "" && !isValidRegister(c) && !isValidVariable(c) && !isValidOtherValue(c)) exitFromExecution("INVALID INSTRUCTION : " + c);

}

// Checking first word for example, mov,add,mul etc.
bool isValidInstruction(string a){
   toLower(a);
   if(a == "mov" || a == "add" || a == "sub" || a == "mul" || a == "div" || a == "xor" || a == "or" || a == "and" || a == "not" || a == "rcl" || a == "rcr" || a == "shl" || a == "shr" || a == "push" || a == "pop" ||a == "nop" || a == "cmp" || a=="jmp" || a == "jz" || a == "jnz" || a == "je"  || a == "jne" || a == "ja" || a == "jae" || a == "jb" || a == "jbe" || a == "jnae" || a == "jnb" || a == "jnbe" || a == "jnc" || a == "jc" || a == "int" || a == "dec" || a == "inc") return true;
   else false;
   //exitFromExecution("INVALID INSTRUCTION : " + a);
}

// Checking if it is a valid register
bool isValidRegister(string a){
   toLower(a);
   a = cleanVariable(a);
   trim(a);
   if(a == "ax" || a == "bx" || a == "cx" || a == "dx" || a == "di" || a == "sp" || a == "si" || a == "bp" || a == "ah" || a == "al" || a == "bh" || a == "bl" || a == "ch" || a == "cl" || a == "dh" || a == "dl") return true;
   else return false;
}

//checking if it is a valid variable
bool isValidVariable(string a){
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      toLower(a);
      if((a.at(0) == 'w'  || a.at(0) == 'b') && (a.at(1) == ' ' || a.at(1) == '.')) a = a.substr(2,a.length());
      else if((a.at(0) == 'w'  || a.at(0) == 'b') && (a.at(1) == '[')) a = a.substr(1,a.length());
      a = cleanVariable(a);
      trim(a);

      for (it = dbVariables.begin(); it != dbVariables.end(); it++) {
         if(a == (*it).name){
            return true;
         }

      }
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
      {
         if(a == (*it2).name){
            return true;
         }
      }
}

//Checking whether it is a valid other value such as 10,10d,300,'a',w[01100d] or not
bool isValidOtherValue(string str1){
   toLower(str1);
   str1 = cleanVariable(str1);
   trim(str1);
   if((str1.at(0) == 'w'  || str1.at(0) == 'b') && (str1.at(1) == ' ' || str1.at(1) == '.')) str1 = str1.substr(2,str1.length());
   else if((str1.at(0) == 'w'  || str1.at(0) == 'b') && (str1.at(1) == '[')) str1 = str1.substr(1,str1.length());
   trim(str1);
   if((48 <= str1.at(0) && str1.at(0) <= 57)){
      if(str1.at(str1.length()-1) == 'd'){
         str1 = str1.substr(0,str1.length()-1);
         try{
            stoi(str1);
            return true;
         }catch(exception e){
            return false;
         }
      }else if((str1.at(str1.length()-1) == 'h')){
         str1 = str1.substr(0,str1.length()-1);
         try{
            hexToDec(str1);
            return true;
         }catch(exception e){
            return false;
         }
      }else if((str1.at(str1.length()-1) == 'b')){
         str1 = str1.substr(0,str1.length()-1);
         
         try{
            binToDec(str1);
            return true;
         }catch(exception e){
            return false;
         }
      }else if((48 <= str1.at(str1.length() -1) && str1.at(str1.length() - 1) <= 57)){
         
         try{
            stoi(str1);
            return true;
         }catch(exception e){
            return false;
         }
      }else{
         return false;
      }
      
   }else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(str1,str1.find_first_of('[')+1)){
         try{
            stoi(cleanVariable(str1));
            return true;
         }catch(exception e){
            return false;
         }
   }else{
      
      if(str1.find('\'') != string::npos && (str1.find_first_of('\'') != str1.find_last_of('\''))){
         return true;
      }else if((str1.find('"') != string::npos || str1.find('\"') != string::npos) && ((str1.find_first_of('\"') != str1.find_last_of('\"')) || (str1.find_first_of('"') != str1.find_last_of('"')))){
         return true;
      }else{
         return false;
      }
      
   }
}

//Checking for errors such as : topla ax,10, mov b var, b 10, push bl, pop 10
void checkForCompatibility(string option,string str1,string str2){
   toLower(option);
   toLower(str1);
   toLower(str2);
      /*

               C  H E C K  for variable definitions db, dw, label, mov, add etc.
               doulbe variable name

      */

      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      std::vector<dbVariable>::iterator firstIt;
      std::vector<dwVariable>::iterator firstIt2;
      unsigned char *pmhl = nullptr;
      unsigned char *pnhl = nullptr;
      unsigned short *pmx = nullptr; 
      unsigned short *pnx = nullptr;   
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      bool isFirstVariableFound1 = false;
      bool isFirstVariableFound2 = false;

      determineReg(&pmx,&pmhl,str1,isFirstVariableFound1,isFirstVariableFound2,firstIt,firstIt2);
      determineReg(&pnx,&pnhl,str2,isVariableFound1,isVariableFound2,it,it2);
         if(option == "shr" || option == "shl" || option == "rcr" || option == "rcl"){
            try{
               if(str2 != "cl" && stoi(str2) >= 32) exitFromExecution("FOR " + option + " ONLY NUMBERS SMALLER THAN 32 OR CL IS ACCEPTED !!!");
            }catch(exception e){
               exitFromExecution("FOR " + option + " ONLY NUMBERS SMALLER THAN 32 OR CL IS ACCEPTED !!!");
            }
         }else if(option == "mov" || option == "add" || option == "sub" || option == "cmp" || option == "or" || option == "and" || option == "xor"){
            if(pmx != nullptr){
               if(checkForB(str2)) exitFromExecution("16 Bit - 8 Bit Error At : " + str2);
               else if(pnx != nullptr) return;
               else if(pnhl != nullptr) exitFromExecution("16 Bit - 8 Bit Error At : " + str2);
               else if(isVariableFound1 || isVariableFound2){
                  if(isItSixteenBitValue(str2)) return;
                  else exitFromExecution("16 Bit - 8 Bit Error");
               }
               else if(determineValueOfInstruction(str2) > 65535) exitFromExecution("ERROR : Result Does not fit into 16 Bit register ! : " + str2);
               else return;
            }else if(pmhl != nullptr){
               if(checkForW(str2)) exitFromExecution("8 Bit - 16 Bit Error At : " + str2);
               else if(pnhl != nullptr) return;
               else if(pnx != nullptr) exitFromExecution("8 Bit - 16 Bit Error At : " + str2);
               else if(isVariableFound1 || isVariableFound2){
                  if(!isItSixteenBitValue(str2)) return;
                  else exitFromExecution("8 Bit - 16 Bit Error At : " + str2);
               }
               else if(determineValueOfInstruction(str2) > 255) exitFromExecution("ERROR : Result Does not fit into 8 Bit register ! : " + str2);
               else return;
            }else if(isFirstVariableFound1){
               if(checkForW(str1)){
                  if(checkForB(str2)) exitFromExecution("16 Bit - 8 Bit Error At : " + str2);
                  else if(pnx != nullptr) return;
                  else if(pnhl != nullptr) exitFromExecution("16 Bit - 8 Bit Error At : " + str2);
                  else if(determineValueOfInstruction(str2) > 65535) exitFromExecution("ERROR : Result Does not fit into 16 Bit register ! : " + str2);
                  else return;
               }else if(!checkForW(str1)){
                  if(checkForW(str2) || checkForB(str2)) exitFromExecution("ERROR : " + str2);
                  else if(pnhl != nullptr) return;
                  else if(pnx != nullptr) exitFromExecution("8 Bit - 16 Bit Error At : " + str2);
                  else if(determineValueOfInstruction(str2) > 255) exitFromExecution("ERROR : Result Does not fit into 8 Bit register ! : " + str2);
                  else return;
               }
            }else if(isFirstVariableFound2){
               if(!checkForB(str1)){
                  if(checkForB(str2)) exitFromExecution("16 Bit - 8 Bit Error At : " + str2);
                  else if(pnx != nullptr) return;
                  else if(pnhl != nullptr) exitFromExecution("16 Bit - 8 Bit Error At : " + str2);
                  else if(determineValueOfInstruction(str2) > 65535) exitFromExecution("ERROR : Result Does not fit into 16 Bit register ! : " + str2);
                  else return;
               }else if(checkForB(str1)){
                  if(checkForW(str2) || checkForB(str2)) exitFromExecution("ERROR  : " + str2);
                  else if(pnhl != nullptr) return;
                  else if(pnx != nullptr) exitFromExecution("8 Bit - 16 Bit Error At : " + str2);
                  else if(determineValueOfInstruction(str2) > 255) exitFromExecution("ERROR : Result Does not fit into 8 Bit register ! : " + str2);
                  else return;
               }
            }else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(str1,str1.find_first_of('[')+1)){
           
           
            }
         }else if(option == "int" || option == "mul" || option == "div" || option == "push" || option == "pop" || option == "inc" || option == "dec"){
            if(option == "push"){
               if(checkForB(str1)) exitFromExecution("ERROR : Only 16 bits can be pushed !!!: " + str1);
               else if(pmx != nullptr) return;
               else if(pmhl != nullptr) exitFromExecution("ERROR : Only 16 bits can be pushed !!!: " + str1);
               else if(isFirstVariableFound2 || isFirstVariableFound1){
                  if(isItSixteenBitValue(str1)) return;
                  else exitFromExecution("ERROR : Only 16 bits can be pushed !!!: " + str1);
               } 
               else if(checkForW(str1) && str1.find('[') != string::npos && str1.find(']') != string::npos) return;
               else if(determineValueOfInstruction(str2) > 65535) exitFromExecution("ERROR : Result Does not fit into 16 Bit register ! : " + str1);
               else return;
            }else if(option == "pop"){
               if(checkForB(str1)) exitFromExecution("ERROR : Pop can be used only with 16 bits !!! : " + str1);
               else if(pmx != nullptr) return;
               else if(pmhl != nullptr) exitFromExecution("ERROR : Pop can be used only with 16 bits !!! : " + str1);
               else if(isFirstVariableFound2 || isFirstVariableFound1){
                  if(isItSixteenBitValue(str1)) return;
                  else exitFromExecution("ERROR : Only 16 bits can be pushed !!! : " + str1);
               }else if(checkForW(str1) && str1.find('[') != string::npos && str1.find(']') != string::npos) return;
               else exitFromExecution("ERROR : Can not pop to : " + str1);
            }else if(option == "int"){
               // Buraya bak tekrar !
            }else if(option == "mul" || option == "div" || option == "not" || option == "inc" || option == "dec"){
               if(checkForB(str1) || checkForW(str1)) return;
               else if(pmx != nullptr) return;
               else if(pmhl != nullptr) return;
               else if(isFirstVariableFound2 || isFirstVariableFound1) return;
               else if((checkForW(str1) || checkForB(str1)) && str1.find('[') != string::npos && str1.find(']') != string::npos) return;
               else exitFromExecution("ERROR : Can not mul ax with : " + str1);
            }
         }



}

// If there is an error this method will print error and exit from program
void exitFromExecution(string paramaterError) {
   cout << paramaterError << endl;
   exit(1);
}

// Checking whether the parameter is a byte or not
bool checkForB(string parameter){
   if((parameter.at(0) == 'b' || parameter.at(0) == 'B') && (parameter.at(1) == ' ' || parameter.at(1) == '.' || parameter.at(1) == '[')) return true;
   else return false;
}

// Checking whether the parameter is a word or not
bool checkForW(string parameter){
   if((parameter.at(0) == 'w' || parameter.at(0) == 'W') && (parameter.at(1) == ' ' || parameter.at(1) == '.' || parameter.at(1) == '[')) return true;
   else return false;
}

// Checking if it is a sixteen bit value or not
bool isItSixteenBitValue(string str1) {
      if(checkForW(str1)) return true;
      else if(checkForB(str1)) return false;
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      std::vector<dbVariable>::iterator firstIt;
      std::vector<dwVariable>::iterator firstIt2;
      unsigned char *pmhl = nullptr;
      unsigned char *pnhl = nullptr;
      unsigned short *pmx = nullptr; 
      unsigned short *pnx = nullptr;   
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;

      determineReg(&pmx,&pmhl,str1,isVariableFound1,isVariableFound2,firstIt,firstIt2);

      if(pmx) return true;
      else if(pmhl) return false;
      else if(isVariableFound1) {
         // if(str1.at(0) == 'w') return true;
         return false;
      }
      else if(isVariableFound2) {
         // if(str1.at(0) == 'b') return false;
         return true;
      }else if((48 <= str1.at(0) && str1.at(0) <= 57)){
            if(str1.at(str1.length()-1) == 'd'){
               str1 = str1.substr(0,str1.length()-1);
               if(stoi(str1) > 255) return true;
               else return false;
            }else if((str1.at(str1.length()-1) == 'h')){
               str1 = str1.substr(0,str1.length()-1);
               if(hexToDec(str1) > 255) return true;
               else return false;
            }else if((str1.at(str1.length()-1) == 'b')){
               str1 = str1.substr(0,str1.length()-1);
               if(binToDec(str1) > 255) return true;
               else return false;
            }else if((48 <= str1.at(str1.length() -1) && str1.at(str1.length() - 1) <= 57)){
               if(stoi(str1) > 255) return true;
               else return false;
            }else{
               return false;
            }
            
         }

      // else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(str1,str1.find_first_of('[')+1) && str1.at(0) == 'w') return true;
      else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(str1,str1.find_first_of('[')+1)) return false;
      else if(str1.find('\'') != string::npos && (str1.find_first_of('\'') != str1.find_last_of('\''))){
         // if(str1.at(0) == 'w') return true;
         return false;
      }
      else if(((str1.find('"') != string::npos) && (str1.find_first_of('"') != str1.find_last_of('"'))) || ((str1.find('\"') != string::npos) && (str1.find_first_of('\"') != str1.find_last_of('\"')))){
         // if(str1.at(0) == 'w') return true;
         return false;
      }else return false;
   // ADD w'a',w"a" ETC. HERE.

}

// DETERMINE Value of Registers and variable. Such as: pmx,pmhl,it,it2,isVariableFound1,isVariableFound2
void determineReg(unsigned short **pmx, unsigned char **pmhl, string& reg,bool& isVariableFound1,bool& isVariableFound2,std::vector<dbVariable>::iterator &it,std::vector<dwVariable>::iterator &it2) {
   string resultReg = reg;
   if(resultReg == "ax"){
      *pmx = pax;
   }else if(resultReg == "bx"){
      *pmx = pbx;
   }else if(resultReg == "cx"){
      *pmx = pcx;
   }else if(resultReg == "dx"){
      *pmx = pdx;
   }else if(resultReg == "ah"){
      *pmhl = pah;
   }else if(resultReg == "al"){
      *pmhl = pal;
   }else if(resultReg == "bh"){
      *pmhl = pbh;
   }else if(resultReg == "bl"){
      *pmhl = pbl;
   }else if(resultReg == "ch"){
      *pmhl = pch;
   }else if(resultReg == "cl"){
      *pmhl = pcl;
   }else if(resultReg == "dh"){
      *pmhl = pdh;
   }else if(resultReg == "dl"){
      *pmhl = pdl;
   }else if(resultReg == "sp"){
      *pmx = psp;
   }else if(resultReg == "si"){
      *pmx = psi;
   }else if(resultReg == "di"){
      *pmx = pdi;
   }else if(resultReg == "bp"){
      *pmx = pbp;
   }else{
      resultReg = cleanVariable(reg);
      trim(resultReg);

      for (it = dbVariables.begin(); it != dbVariables.end(); it++) {
         if(resultReg.find((*it).name) != string::npos){
            isVariableFound1 = true;
            break;
         }

      }
      if(!isVariableFound1){
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
      {
         if(resultReg.find((*it2).name) != string::npos){
            isVariableFound2 = true;
            break;
         }
      }
      }
   }

   



}

// DETERMINE VALUE OF INSTRUCTION and return it.
// ax,al,msg,1,1d,1h,'1',"1",[msg],[ax],[0090h],[0090]
int determineValueOfInstruction(string reg) {
   if(reg == "") return 0;
   toLower(reg);
   string resultReg = reg;
   int result = 0;
   std::vector<dbVariable>::iterator it;
   std::vector<dwVariable>::iterator it2;

   bool isVariableFound1 = false;
   bool isVariableFound2 = false;

   if(resultReg == "ax"){
      result = *pax;
   }else if(resultReg == "bx"){
      result = *pbx;
   }else if(resultReg == "cx"){
      result = *pcx;
   }else if(resultReg == "dx"){
      result = *pdx;
   }else if(resultReg == "ah"){
      result = *pah;
   }else if(resultReg == "al"){
      result = *pal;
   }else if(resultReg == "bh"){
      result = *pbh;
   }else if(resultReg == "bl"){
      result = *pbl;
   }else if(resultReg == "ch"){
      result = *pch;
   }else if(resultReg == "cl"){
      result = *pcl;
   }else if(resultReg == "dh"){
      result = *pdh;
   }else if(resultReg == "dl"){
      result = *pdl;
   }else if(resultReg == "sp"){
      result = *psp;
   }else if(resultReg == "si"){
      result = *psi;
   }else if(resultReg == "di"){
      result = *pdi;
   }else if(resultReg == "bp"){
      result = *pbp;
   }else{
      resultReg = cleanVariable(reg);
      trim(resultReg);

      for (it = dbVariables.begin(); it != dbVariables.end(); it++)
      {
         if(resultReg.find((*it).name) != string::npos){
            isVariableFound1 = true;
            break;
         }

      }
      if(!isVariableFound1){
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
      {
         if(resultReg.find((*it2).name) != string::npos){
            isVariableFound2 = true;
            break;
         }
      }
      }

    if(isVariableFound1){
         if(!isItSixteenBitValue(reg)){
            result = memory[(*it).address];
         }else{
            result = memory[(*it).address] + memory[(*it).address+1] * pow(2,8); 
         }
      }else if(isVariableFound2){
         if(!isItSixteenBitValue(reg)){
            result = memory[(*it2).address];
         }else{
            result = memory[(*it2).address] + memory[(*it2).address+1] * pow(2,8); 
         }
      }else{
         result = getOtherValue(reg);
      }
      
   }
   return result;
}

// determineValueOfInstruction Calls This Function For other values such as 1,1d,1h,'1',"1",[0090h],[0090]
int getOtherValue(string str1) {
   if(str1 == "") return 0;
   int result = 0;
   char character;
   bool isItSixteenBitt = isItSixteenBitValue(str1);
   if((str1.at(0) == 'w'  || str1.at(0) == 'b') && (str1.at(1) == ' ' || str1.at(1) == '.')) str1 = str1.substr(2,str1.length());
   else if((str1.at(0) == 'w'  || str1.at(0) == 'b') && (str1.at(1) == '[')) str1 = str1.substr(1,str1.length());
   trim(str1);

   if((48 <= str1.at(0) && str1.at(0) <= 57)){
      if(str1.at(str1.length()-1) == 'd'){
         str1 = str1.substr(0,str1.length()-1);
         result = stoi(str1);
      }else if((str1.at(str1.length()-1) == 'h')){
         str1 = str1.substr(0,str1.length()-1);
         result = hexToDec(str1);
      }else if((str1.at(str1.length()-1) == 'b')){
         str1 = str1.substr(0,str1.length()-1);
         result = binToDec(str1);
      }else if((48 <= str1.at(str1.length() -1) && str1.at(str1.length() - 1) <= 57)){
         result = stoi(str1);
      }else{
         exitFromExecution("CAN NOT BE PARSED !!! " + str1);
      }
      
   }else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(str1,str1.find_first_of('[')+1)){
      int address = stoi(cleanVariable(str1));
      if(address > 65535 || address < 0) exitFromExecution("ERROR : THERE IS NOT SUCH A LOCATION !!!");
      if(isItSixteenBitt){
        result = memory[address] + memory[address+1] * pow(2,8);
      }else{
         result = memory[address];
      }
   }else{
      
      if(str1.find('\'') != string::npos && (str1.find_first_of('\'') != str1.find_last_of('\''))){
         character = str1.at(str1.find_first_of('\'')+1);
      }else if((str1.find('"') != string::npos || str1.find('\"') != string::npos) && ((str1.find_first_of('\"') != str1.find_last_of('\"')) || (str1.find_first_of('"') != str1.find_last_of('"')))){
         character = (str1.find_first_of('\"') != string::npos) ? str1.at(str1.find_first_of('\"')+1) : str1.at(str1.find_first_of('"')+1);
      }else{
         exitFromExecution("CAN NOT BE PARSED !!! " + str1);
      }
      result = character;
   }
   if(isItSixteenBitt){
      return getSixteenBitValueOfNumber(result);
   }else{
      return getRightEightBitValueOfNumber(result);
   }
}

// Returns right 8 bit of the value
int getRightEightBitValueOfNumber(int number){
   string temp = decToBin(number);
   return binToDec(temp.substr(temp.length()-8,temp.length()));
}

// Returns bits between right value.length()-16 and value.length()-8 characters of the value
int getLeftEightBitValueOfNumber(int number){
   string temp = decToBin(number);
   temp = temp.substr(temp.length()-16,temp.length());
   return binToDec(temp.substr(0,temp.length()-8));
}

// Returns right 16 bit of the value
int getSixteenBitValueOfNumber(int number){
   string temp = decToBin(number);
   return binToDec(temp.substr(temp.length()-16,temp.length()));
}

//     Determine and calls the funstions according to input line, and sends parameters which are not nullptr
void instructionOptions(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,std::vector<dbVariable>::iterator firstIt,std::vector<dwVariable>::iterator firstIt2,bool& isFirstVariableFound1,bool& isFirstVariableFound2,string& str1,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl,string option) {
         if(pmx != nullptr){
            moveValueToReg(&pmx,pnx,it,it2,str2,str3,option);
         }else if(pmhl != nullptr){
            moveValueToReg(&pmhl,pnhl,it,it2,str2,str3,option);
         }else if(isFirstVariableFound1){
            if(pnhl != nullptr) moveValueToVariable(firstIt,pnhl,it,it2,str1,str2,str3,option);
            else moveValueToVariable(firstIt,pnx,it,it2,str1,str2,str3,option);
         }else if(isFirstVariableFound2){
            if(pnhl != nullptr) moveValueToVariable(firstIt2,pnhl,it,it2,str1,str2,str3,option);
            else moveValueToVariable(firstIt2,pnx,it,it2,str1,str2,str3,option);
         }else if(str1.find('[') != string::npos && str1.find(']') != string::npos && isDigitDecimal(str1,str1.find_first_of('[')+1)){
            instructionForBrakets(str1,str2,str3,option);
         }  
}


// If first parameter is a register then this function will put value to that register
template <class regOne, class regTwo> 
void moveValueToReg(regOne** firstReg, regTwo* secondReg,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,string& str2,string& str3,string option){
   
   int resultOfInstruction = 0;
   if(secondReg == nullptr) resultOfInstruction = (str3 == "offset") ? getVariableAddress(str2) : determineValueOfInstruction(str2);
   else resultOfInstruction = *secondReg;
   if(option == "mov"){
      **firstReg = resultOfInstruction;
   }else if(option == "add"){
      checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"add");
      **firstReg += resultOfInstruction;
   }else if(option == "sub"){
      checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"sub");
      **firstReg -= resultOfInstruction;
   }else if(option == "or"){
      checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"or");
      **firstReg |= resultOfInstruction;
   }else if(option == "and"){
      checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"and");
      **firstReg &= resultOfInstruction;
   }else if(option == "xor"){
      checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"xor");
      **firstReg ^= resultOfInstruction;
   }else if(option == "shr"){
      checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"shr");
      for (int i = 0; i < resultOfInstruction; i++) {
         cf = **firstReg & 1;
         **firstReg >>= 1;
         // printBits(ax);
      }
   }else if(option == "shl"){
      checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"shl");
      for (int i = 0; i < resultOfInstruction; i++) {
         cf = decToBin(**firstReg).at(0)-'0';
         **firstReg <<= 1;
         // printBits(ax);
      }
   }else if(option == "rcr"){
      checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"rcr");
      for (int i = 0; i < resultOfInstruction; i++) {
         unsigned short temp = cf << sizeof(**firstReg) * 8 - 1;
         cf = **firstReg & 1;
         **firstReg >>= 1;
         **firstReg |= temp;
      }
   }else if(option == "rcl"){
      checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"rcl");
      for (int i = 0; i < resultOfInstruction; i++) {
         bool temp = cf;
         cf = decToBin(**firstReg).at(0)-'0';
         **firstReg <<= 1;
         **firstReg |= temp;
         // printBits(*pcl);
      }
   }else if(option == "cmp"){
      checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"sub");
   }
}

// If first parameter is a variable then this function will put value to that variable
template <class regOne, class regTwo> 
void moveValueToVariable(regOne& firstIt,regTwo *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,string& str1,string& str2,string& str3,string option){
   unsigned short result = 0;
   string type = "";
   int bit = 0;
   int resultOfInstruction = 0;
   if(pnx == nullptr) resultOfInstruction = (str3 == "offset") ? getVariableAddress(str2) : determineValueOfInstruction(str2);
   else resultOfInstruction = *pnx;
   bit = isItSixteenBitValue(str1) ? 16 : 8; 
   type = bit == 16 ? "dw" : "db";
   
   if(bit == 16) result = memory[(*firstIt).address] + memory[(*firstIt).address + 1] * pow(2,8);
   else result = memory[(*firstIt).address] ;
   checkAndSetFlags(result,resultOfInstruction,bit,option);

   if(option == "mov"){ result = resultOfInstruction; setMemoryForDbAndDw((*firstIt).address,result,type);}
   else if(option == "add"){ result += resultOfInstruction; setMemoryForDbAndDw((*firstIt).address,result,type);}
   else if(option == "sub"){ result -= resultOfInstruction; setMemoryForDbAndDw((*firstIt).address,result,type);}
   else if(option == "or"){ result |= resultOfInstruction; setMemoryForDbAndDw((*firstIt).address,result,type);}
   else if(option == "and"){ result &= resultOfInstruction; setMemoryForDbAndDw((*firstIt).address,result,type);}
   else if(option == "xor"){ result ^= resultOfInstruction; setMemoryForDbAndDw((*firstIt).address,result,type);}
   else if(option == "shr"){
      for (int i = 0; i < resultOfInstruction; i++) {
         cf = (*(*firstIt).value) & 1;
         (*(*firstIt).value) >>= 1;
      }
      setMemoryForDbAndDw((*firstIt).address,*(*firstIt).value,(*firstIt).type);
   }else if(option == "shl"){
      for (int i = 0; i < resultOfInstruction; i++) {
         cf = decToBin((*(*firstIt).value)).at(0)-'0';
         (*(*firstIt).value) <<= 1;
      }
      setMemoryForDbAndDw((*firstIt).address,*(*firstIt).value,(*firstIt).type);
   }else if(option == "rcr"){
      for (int i = 0; i < resultOfInstruction; i++) {
         unsigned short temp = cf << sizeof(*(*firstIt).value) * 8 - 1;
         cf = (*(*firstIt).value) & 1;
         (*(*firstIt).value) >>= 1;
         (*(*firstIt).value) |= temp;
      }
      setMemoryForDbAndDw((*firstIt).address,*(*firstIt).value,(*firstIt).type);
   }else if(option == "rcl"){
      for (int i = 0; i < resultOfInstruction; i++) {
         bool temp = cf;
         cf = decToBin(*(*firstIt).value).at(0)-'0';
         (*(*firstIt).value) <<= 1;
         (*(*firstIt).value) |= temp;
         // printBits(ax);
      }
      setMemoryForDbAndDw((*firstIt).address,*(*firstIt).value,(*firstIt).type);
   }else if(option == "cmp"){
      checkAndSetFlags(*(*firstIt).value,resultOfInstruction,bit,"sub");
   }

}

// If first parameter is a memory location then this function will put value to that memory location
void instructionForBrakets(string str1,string str2,string str3,string option) {
   int result = 0;
   unsigned short temp = 0;
   string type = "";
   int bit = 0;
   result = (str3 == "offset") ? getVariableAddress(str2) : determineValueOfInstruction(str2);
   int address = stoi(cleanVariable(str1));

   bit = isItSixteenBitValue(str1) ? 16 : 8; 
   type = bit == 16 ? "dw" : "db";
   
   if(bit == 16) temp = memory[address] + memory[address + 1] * pow(2,8); 
   else temp = memory[address] ;
   checkAndSetFlags(temp,result,bit,option);



   if(option == "mov"){ temp = result; setMemoryForDbAndDw(address,temp,type); }
   else if(option == "add"){ temp += result; setMemoryForDbAndDw(address,temp,type); }
   else if(option == "sub") { temp -= result; setMemoryForDbAndDw(address,temp,type); }
   else if(option == "or") { temp |= result; setMemoryForDbAndDw(address,temp,type); }
   else if(option == "and") { temp &= result; setMemoryForDbAndDw(address,temp,type); }
   else if(option == "xor") { temp ^= result; setMemoryForDbAndDw(address,temp,type); }
   else if(option == "shr"){
         for (int i = 0; i < result; i++) {
            cf = temp & 1;
            temp >>= 1;
         }
         setMemoryForDbAndDw(address,temp,type);
   }else if(option == "shl"){
      if(isItSixteenBitValue(str1)){
         for (int i = 0; i < result; i++) {
            cf = decToBin(temp).at(0)-'0';
            temp <<= 1;
         }
         setMemoryForDbAndDw(address,temp,type);
      }else{
         for (int i = 0; i < result; i++) {
               cf = decToBin(memory[address]).at(0)-'0';
               memory[address] <<= 1;
            } 
      }
   }else if(option == "rcr"){
      if(bit == 16){
         for (int i = 0; i < result; i++) {
            unsigned short shiftedValueOfCf = cf << sizeof(temp) * 8 - 1;
            cf = temp & 1;
            temp >>= 1;
            temp |= shiftedValueOfCf;
         } 
         setMemoryForDbAndDw(address,temp,type);
      }else{
         for (int i = 0; i < result; i++) {
            unsigned short temp2 = cf << sizeof(memory[address]) * 8 - 1;
            cf = memory[address] & 1;
            memory[address] >>= 1;
            memory[address] |= temp2;
         } 
      }

   }else if(option == "rcl"){
      if(isItSixteenBitValue(str1)){
         for (int i = 0; i < result; i++) {
            bool oldValueOfCf = cf;
            cf = decToBin(temp).at(0)-'0';
            temp <<= 1;
            temp |= oldValueOfCf;
         }         
         setMemoryForDbAndDw(address,temp,type);
      }else{
         for (int i = 0; i < result; i++) {
            bool temp2 = cf;
            cf = decToBin(memory[address]).at(0)-'0';
            memory[address] <<= 1;
            memory[address] |= temp2;
            }
      }

   }else if(option == "cmp"){
      //
   }

}


// put the value to an appropriate Memory address
template <class typeOfVariableValue> 
void setMemoryForDbAndDw(int address,typeOfVariableValue variableValue,string typeOfVariable) {
   if(typeOfVariable == "dw"){
      memory[address] = getRightEightBitValueOfNumber(variableValue);
      memory[address+1] = getLeftEightBitValueOfNumber(variableValue);
   }else{
      if(variableValue > 255) {
         cout << "Paramater is too large to fit into memory !!!" << endl;
         exit(1);
      }
      memory[address] = variableValue;
   }
}

// Checking instruction and determine flags according to result of execution
void checkAndSetFlags(int number1,int number2,int bit,string option) {
   unsigned char num1EightBit = number1;
   unsigned char num2EightBit = number2;
   unsigned short num1SixteenBit = number1;
   unsigned short num2SixteenBit = number2;
   unsigned char eightBitResult = 0;
   unsigned short sixteenBitResult = 0;
   bool isFirstDigitOne1 = decToBin(number1).at(0) == '1';
   bool isFirstDigitOne2 = decToBin(number2).at(0) == '1';
   if(option == "add"){
      if(bit == 8){
         eightBitResult = num1EightBit + num2EightBit;
         if(decToBin(eightBitResult).at(0) == '1') sf = 1;
         else sf = 0;
         if(num1EightBit + num2EightBit > 255) cf = 1;
         else cf = 0;
         if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         else of = 0;
         if(eightBitResult == 0) zf = 1;
         else zf = 0;
         
         unsigned char number1Binary = binToDec(decToBin(num1EightBit).substr(4,8));
         unsigned char number2Binary = binToDec(decToBin(num2EightBit).substr(4,8));
         if(number1Binary + number2Binary > 15) af = 1;
         else af = 0;
         // if(countSpecificCharacter(decToBin(eightBitResult),'1') % 2 == 0) pf = 1;
         // else pf = 0;
      }else if(bit == 16){
         sixteenBitResult = num1SixteenBit + num2SixteenBit;
         if(decToBin(sixteenBitResult).at(0) == '1') sf = 1;
         else sf = 0;
         if(num1SixteenBit+num2SixteenBit > 65535) cf = 1;
         else cf = 0;
         if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         else of = 0;
         if(sixteenBitResult == 0) zf = 1;
         else zf = 0;
         unsigned char number1Binary = binToDec(decToBin(num1SixteenBit).substr(12,16));
         unsigned char number2Binary = binToDec(decToBin(num2SixteenBit).substr(12,16));
         if(number1Binary + number2Binary > 15) af = 1;
         else af = 0;
      }

   }else if(option == "sub" || option == "cmp"){
       if(bit == 8){
         eightBitResult = num1EightBit - num2EightBit;
         if(decToBin(eightBitResult).at(0) == '1') sf = 1;
         else sf = 0;
         if(num1EightBit < num2EightBit) cf = 1;
         else cf = 0;
         // if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         // else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         // else of = 0;
         if(eightBitResult == 0) zf = 1;
         else zf = 0;
         
         unsigned char number1Binary = binToDec(decToBin(num1EightBit).substr(4,8));
         unsigned char number2Binary = binToDec(decToBin(num2EightBit).substr(4,8));
         if(number1Binary < number2Binary) af = 1;
         else af = 0;
         // if(countSpecificCharacter(decToBin(eightBitResult),'1') % 2 == 0) pf = 1;
         // else pf = 0;
      }else if(bit == 16){
         sixteenBitResult = num1SixteenBit - num2SixteenBit;
         if(decToBin(sixteenBitResult).at(0) == '1') sf = 1;
         else sf = 0;
         if(num1SixteenBit < num2SixteenBit) cf = 1;
         else cf = 0;
         // if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         // else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         // else of = 0;
         if(sixteenBitResult == 0) zf = 1;
         else zf = 0;
         unsigned char number1Binary = binToDec(decToBin(num1SixteenBit).substr(12,16));
         unsigned char number2Binary = binToDec(decToBin(num2SixteenBit).substr(12,16));
         if(number1Binary < number2Binary) af = 1;
         else af = 0;
      }
   }else if(option == "div"){
         if(bit == 8){
            if(number1 / number2 > 255 || number1 % number2 > 255) exitFromExecution("ERROR : DIVISION OVERFLOW !!!");
         }else if(bit == 16){
            if(number1 / number2 > 65535 || number1 % number2 > 65535) exitFromExecution("ERROR : DIVISION OVERFLOW !!!");
         }
   }else if(option == "inc"){
      if(bit == 8){
         eightBitResult = num1EightBit + 1;
         if(decToBin(eightBitResult).at(0) == '1') sf = 1;
         else sf = 0;
         if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         else of = 0;
         if(eightBitResult == 0) zf = 1;
         else zf = 0;
         
         unsigned char number1Binary = binToDec(decToBin(num1EightBit).substr(4,8));
         if(number1Binary + 1 > 15) af = 1;
         else af = 0;
         // if(countSpecificCharacter(decToBin(eightBitResult),'1') % 2 == 0) pf = 1;
         // else pf = 0;
      }else if(bit == 16){
         sixteenBitResult = num1SixteenBit + 1;
         if(decToBin(sixteenBitResult).at(0) == '1') sf = 1;
         else sf = 0;

         if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         else of = 0;
         
         if(sixteenBitResult == 0) zf = 1;
         else zf = 0;
         
         unsigned char number1Binary = binToDec(decToBin(num1SixteenBit).substr(12,16));
         if(number1Binary + 1 > 15) af = 1;
         else af = 0;
      }

   }else if(option == "dec"){
      if(bit == 8){
         eightBitResult = num1EightBit - 1;
         if(decToBin(eightBitResult).at(0) == '1') sf = 1;
         else sf = 0;
         if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         else of = 0;

         if(eightBitResult == 0) zf = 1;
         else zf = 0;
         
         unsigned char number1Binary = binToDec(decToBin(num1EightBit).substr(4,8));
         if(decToBin(num1EightBit).substr(4,8).at(0) =='0' && decToBin(num1EightBit - 1).substr(4,8).at(0) =='1') af = 1;
         else af = 0;
         // if(countSpecificCharacter(decToBin(eightBitResult),'1') % 2 == 0) pf = 1;
         // else pf = 0;
      }else if(bit == 16){
         sixteenBitResult = num1SixteenBit - 1;
         if(decToBin(sixteenBitResult).at(0) == '1') sf = 1;
         else sf = 0;

         if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         else of = 0;
         
         if(sixteenBitResult == 0) zf = 1;
         else zf = 0;
         
         unsigned char number1Binary = binToDec(decToBin(num1SixteenBit).substr(12,16));
         if(decToBin(num1SixteenBit).substr(12,16).at(0) =='0' && decToBin(num1SixteenBit - 1).substr(12,16).at(0) =='1') af = 1;
         else af = 0;
      }

   }else if(option == "xor"){
      of = cf = 0;
      if(bit == 8){
         eightBitResult = num1EightBit ^ num2EightBit;
         if(decToBin(eightBitResult).at(0) == '1') sf = 1;
         else sf = 0;

         if(eightBitResult == 0) zf = 1;
         else zf = 0;
         
         unsigned char number1Binary = binToDec(decToBin(num1EightBit).substr(4,8));
         unsigned char number2Binary = binToDec(decToBin(num2EightBit).substr(4,8));
         if(number1Binary ^ number2Binary > 15) af = 1;
         else af = 0;
         // if(countSpecificCharacter(decToBin(eightBitResult),'1') % 2 == 0) pf = 1;
         // else pf = 0;
      }else if(bit == 16){
         sixteenBitResult = num1SixteenBit ^ num2SixteenBit;
         if(decToBin(sixteenBitResult).at(0) == '1') sf = 1;
         else sf = 0;

         if(sixteenBitResult == 0) zf = 1;
         else zf = 0;

         unsigned char number1Binary = binToDec(decToBin(num1SixteenBit).substr(12,16));
         unsigned char number2Binary = binToDec(decToBin(num2SixteenBit).substr(12,16));
         if(number1Binary ^ number2Binary > 15) af = 1;
         else af = 0;
      }

   }else if(option == "or"){
      of = cf = 0;
      if(bit == 8){
         eightBitResult = num1EightBit | num2EightBit;
         if(decToBin(eightBitResult).at(0) == '1') sf = 1;
         else sf = 0;

         if(eightBitResult == 0) zf = 1;
         else zf = 0;
         
         unsigned char number1Binary = binToDec(decToBin(num1EightBit).substr(4,8));
         unsigned char number2Binary = binToDec(decToBin(num2EightBit).substr(4,8));
         if(number1Binary | number2Binary > 15) af = 1;
         else af = 0;
         // if(countSpecificCharacter(decToBin(eightBitResult),'1') % 2 == 0) pf = 1;
         // else pf = 0;
      }else if(bit == 16){
         sixteenBitResult = num1SixteenBit | num2SixteenBit;
         if(decToBin(sixteenBitResult).at(0) == '1') sf = 1;
         else sf = 0;

         if(sixteenBitResult == 0) zf = 1;
         else zf = 0;

         unsigned char number1Binary = binToDec(decToBin(num1SixteenBit).substr(12,16));
         unsigned char number2Binary = binToDec(decToBin(num2SixteenBit).substr(12,16));
         if(number1Binary | number2Binary > 15) af = 1;
         else af = 0;
      }
   }else if(option == "and"){
      of = cf = 0;
      if(bit == 8){
         eightBitResult = num1EightBit & num2EightBit;
         if(decToBin(eightBitResult).at(0) == '1') sf = 1;
         else sf = 0;

         if(eightBitResult == 0) zf = 1;
         else zf = 0;
         
         unsigned char number1Binary = binToDec(decToBin(num1EightBit).substr(4,8));
         unsigned char number2Binary = binToDec(decToBin(num2EightBit).substr(4,8));
         if(number1Binary & number2Binary > 15) af = 1;
         else af = 0;
         // if(countSpecificCharacter(decToBin(eightBitResult),'1') % 2 == 0) pf = 1;
         // else pf = 0;
      }else if(bit == 16){
         sixteenBitResult = num1SixteenBit & num2SixteenBit;
         if(decToBin(sixteenBitResult).at(0) == '1') sf = 1;
         else sf = 0;

         if(sixteenBitResult == 0) zf = 1;
         else zf = 0;

         unsigned char number1Binary = binToDec(decToBin(num1SixteenBit).substr(12,16));
         unsigned char number2Binary = binToDec(decToBin(num2SixteenBit).substr(12,16));
         if(number1Binary & number2Binary > 15) af = 1;
         else af = 0;
      }
   }else if(option == "shl"){
      if(bit == 8){
         eightBitResult = num1EightBit << num2EightBit;
         if(decToBin(eightBitResult).at(0) == '1') sf = 1;
         else sf = 0;
         // if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         // else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         // else of = 0;
         if(eightBitResult == 0) zf = 1;
         else zf = 0;
         
         // unsigned char number1Binary = binToDec(decToBin(num1EightBit).substr(4,8));
         // unsigned char number2Binary = binToDec(decToBin(num2EightBit).substr(4,8));
         // if(number1Binary < number2Binary) af = 1;
         // else af = 0;
         // if(countSpecificCharacter(decToBin(eightBitResult),'1') % 2 == 0) pf = 1;
         // else pf = 0;
      }else if(bit == 16){
         sixteenBitResult = num1SixteenBit << num2SixteenBit;
         if(decToBin(sixteenBitResult).at(0) == '1') sf = 1;
         else sf = 0;
         // if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         // else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         // else of = 0;
         if(sixteenBitResult == 0) zf = 1;
         else zf = 0;
         // unsigned char number1Binary = binToDec(decToBin(num1SixteenBit).substr(12,16));
         // unsigned char number2Binary = binToDec(decToBin(num2SixteenBit).substr(12,16));
         // if(number1Binary < number2Binary) af = 1;
      }
   }else if(option == "shr"){
      if(bit == 8){
         eightBitResult = num1EightBit >> num2EightBit;
         if(decToBin(eightBitResult).at(0) == '1') sf = 1;
         else sf = 0;
         // if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         // else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         // else of = 0;
         if(eightBitResult == 0) zf = 1;
         else zf = 0;
         
         // unsigned char number1Binary = binToDec(decToBin(num1EightBit).substr(4,8));
         // unsigned char number2Binary = binToDec(decToBin(num2EightBit).substr(4,8));
         // if(number1Binary < number2Binary) af = 1;
         // else af = 0;
         // if(countSpecificCharacter(decToBin(eightBitResult),'1') % 2 == 0) pf = 1;
         // else pf = 0;
      }else if(bit == 16){
         sixteenBitResult = num1SixteenBit >> num2SixteenBit;
         if(decToBin(sixteenBitResult).at(0) == '1') sf = 1;
         else sf = 0;
         // if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         // else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         // else of = 0;
         if(sixteenBitResult == 0) zf = 1;
         else zf = 0;
         // unsigned char number1Binary = binToDec(decToBin(num1SixteenBit).substr(12,16));
         // unsigned char number2Binary = binToDec(decToBin(num2SixteenBit).substr(12,16));
         // if(number1Binary < number2Binary) af = 1;
      }
   }

}




// Sets Variable values
void setVariableValue(string variableName,int value){
   variableName = cleanVariable(variableName);
   std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      for (it = dbVariables.begin(); it != dbVariables.end(); it++) {
         if(variableName == (*it).name){
            *(*it).value = value;
            isVariableFound1 = true;
            break;
         }

      }
      if(!isVariableFound1){
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++) {
         if(variableName == (*it2).name){
            *(*it2).value = value;
            isVariableFound2 = true;
            break;
         }
      }
      }
}


// With variable name,this method determines variable address and returns it
int getVariableAddress(string& variable){
   string resultVariable = "";
   resultVariable = cleanVariable(variable);
   std::vector<dbVariable>::iterator it;
   std::vector<dwVariable>::iterator it2;
   bool isVariableFound1 = false;
   bool isVariableFound2 = false;
   for (it = dbVariables.begin(); it != dbVariables.end(); it++) {
      if(resultVariable == (*it).name){
         isVariableFound1 = true;
         break;
      }

   }
   if(!isVariableFound1){
   for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
   {
      if(resultVariable == (*it2).name){
         isVariableFound2 = true;
         break;
      }
   }
   }
   if(isVariableFound1) return (*it).address;
   else return (*it2).address;
}

// With memory address, this method determines variable value and returns it
int getVariableValueFromMemoryAddress(string address) {
   return memory[stoi(cleanVariable(address))];
}

// With memory address, this method determines variable name and returns it
string getVariableNameFromVariableAddress(string address){
   int resultAddress = 0;
   resultAddress = stoi(cleanVariable(address));
   std::vector<dbVariable>::iterator it;
   std::vector<dwVariable>::iterator it2;
   bool isVariableFound1 = false;
   bool isVariableFound2 = false;
   for (it = dbVariables.begin(); it != dbVariables.end(); it++) {
      if(resultAddress == (*it).address){
         return (*it).name;
      }

   }
   for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++) {
      if(resultAddress == (*it2).address){
         return (*it2).name;
      }
   }
   return "";
}



//REMOVE [,],h,d,b  FROM WORDS , THIS METHOD WILL RETURN A DECIMAL VALUE
string cleanVariable(string variable) {
   if(variable.find('[') != string::npos && variable.find(']') != string::npos){
      variable = variable.substr(variable.find_first_of('[')+1,variable.length());
      variable = variable.substr(0,variable.find_last_of(']'));
      if((variable.at(variable.length()-1) == 'h' || variable.at(variable.length()-1) == 'd' || variable.at(variable.length()-1) == 'b') && variable.at(0) >= 48 && variable.at(0) <= 57) {
         if(variable.at(variable.length()-1) == 'h'){
            variable = variable.substr(0,variable.length()-1);
            stringstream ssm;
            ssm << hexToDec(variable);
            variable = ssm.str(); 
         }else if(variable.at(variable.length()-1) == 'b'){
            variable = variable.substr(0,variable.length()-1);
            stringstream ssm;
            ssm << binToDec(variable);
            variable = ssm.str(); 
         }else{
            variable = variable.substr(0,variable.length()-1);
         }
      }
      
   } 
   return variable;
}

// Separates Two parameter Words
void twoWordsComma(istringstream& linestream,string& secondWord, string& thirdWord){
   getLinestreamLine(linestream,secondWord,',');
   getLinestreamLine(linestream,thirdWord,'~');
}

// Separate Three parameter Words
void thirdWordsComma(istringstream& linestream,string& secondWord, string& thirdWord, string& forthWord){
   getLinestreamLine(linestream,secondWord,',');
   getLinestreamLine(linestream,forthWord,' ');
   getLinestreamLine(linestream,thirdWord,',');
}




// PRINT VARIABLES
void printVariables(){
   std::vector<dbVariable>::iterator it;
   std::vector<dwVariable>::iterator it2;
   
   
   for (it = dbVariables.begin(); it != dbVariables.end(); it++)
   {
      cout << "DB : " << (*it).name  <<endl;

   }

   for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
   {
      cout  << "DW : " << (*it2).name << endl;
   }
}


// PRINT LABELS
void printLabels(){

   std::vector<string>::iterator it;
   
   for (int i = 0; i < labels.size(); i++)
   {
   for (it = labels.at(i).content.begin(); it != labels.at(i).content.end(); it++)
   {
      cout << "LABEL NAME : " << labels.at(i).name << " - LABEL CONTENT : " << (*it) << endl;

   }
      }
}

// Returns index of labels according to their name
int getIndexOfLabel(string & labelName) {

   std::vector<string>::iterator it;
   
   for (int i = 0; i < labels.size(); i++) {
      if(labels.at(i).name == labelName){
         return i;
         }
      }
      return -1;
}



// Trim words
inline std::string trim(std::string& str) {
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    str.erase(str.find_last_not_of(' ')+1);         //surfixing spaces
    return str;
}

//Gets line from input,trims it,remove this character '\r' and lastly lowers it
void getLineTrimLower(ifstream& inFile,string& firstLine){
   getline(inFile,firstLine);
   trim(firstLine);
   if(firstLine.find("\r") != string::npos) firstLine = firstLine.substr(0,firstLine.length()-1);
   toLower(firstLine);
   
}

// get words from line and trims it
inline string getLinestreamLine(istringstream& linestream,string& word,char option) {
   if(option == '~'){
      getline(linestream,word);
   }else{
      do{
         getline(linestream,word,option);
         if(!linestream) break;
      }while(word == "");
   }
   trim(word);
   return word;
}

// counts specific characters in the word
int countSpecificCharacter(string str, char character) {
   int count = 0;
   for (int i = 0; i < str.length(); i++) {
      if(str.at(i) == character) count++;
   }
   return count;
   
}



// Lowers the word
void toLower(string& firstLine){
   if(isint20h){
      istringstream linestream(firstLine);
      string a,b,c = "";
      getLinestreamLine(linestream,a,' ');
      getLinestreamLine(linestream,b,' ');
      getLinestreamLine(linestream,c,'~');
      transform(b.begin(), b.end(), b.begin(), ::tolower);
      if(b == "db" || b == "dw"){
         transform(a.begin(), a.end(), a.begin(), ::tolower);
         if(c == "' '") c = "32";
         firstLine = a + " " + b + " " + c;
      }

   }
   // FOR UPPER CASE CHARACTERS
   else if((firstLine.find('\'') != string::npos && firstLine.find_first_of('\'') != firstLine.find_last_of('\'')) || (firstLine.find('"') != string::npos && firstLine.find_first_of('"') != firstLine.find_last_of('"'))){
      istringstream linestream(firstLine);
      string a,b,c = "";
      getLinestreamLine(linestream,a,' ');
      if(firstLine.find("' '") != string::npos){
         if(isItOneWordInstruction(a))
            getLinestreamLine(linestream,b,'~');
         else
            getLinestreamLine(linestream,b,' ');
      }else{
         getLinestreamLine(linestream,b,' ');
      }
      getLinestreamLine(linestream,c,'~');
      if((b.find('\'') != string::npos && b.find_first_of('\'') != b.find_last_of('\'')) || (b.find('"') != string::npos && b.find_first_of('"') != b.find_last_of('"'))){
         transform(a.begin(), a.end(), a.begin(), ::tolower);
         transform(c.begin(), c.end(), c.begin(), ::tolower);
         if(b == "' '") b = "32";
         firstLine = a + " " + b + " " + c;
      }else if((c.find('\'') != string::npos && c.find_first_of('\'') != c.find_last_of('\'')) || (c.find('"') != string::npos && c.find_first_of('"') != c.find_last_of('"'))){
         transform(a.begin(), a.end(), a.begin(), ::tolower);
         transform(b.begin(), b.end(), b.begin(), ::tolower);
         if(c == "' '") c = "32";
         firstLine = a + " " + b + " " + c;
      }
   }
   else{
         transform(firstLine.begin(), firstLine.end(), firstLine.begin(), ::tolower);
      }
      trim(firstLine);
}
// Uppers the word
void toUpper(string& firstLine){
   transform(firstLine.begin(), firstLine.end(), firstLine.begin(), ::toupper);
}

// Determines if it is a one word instruction. For example, push, pop,inc,dec,mul and so on.
bool isItOneWordInstruction(string a) {
   transform(a.begin(), a.end(), a.begin(), ::tolower);
   if(a == "push" || a == "pop" || a == "inc" || a == "dec" || a == "int" || a == "not" || a == "int" || a == "div"  || a == "mul" || a == "jmp" || a == "jz" || a == "je" || a == "jnz" || a == "jne" || a == "jja" || a == "jnbe" || a == "jae" || a == "jnc" || a == "jnb" || a == "jb" || a == "jnae" || a == "jc" || a == "jbe" || a == "jna") return true;
   return false;
}

// Check if the line containes space
bool checkSpace(string& line){
   return line.find(' ') != string::npos;
}

// Check if the line containes Quotation Marks
bool checkQuotationMarks(string& line){
   return line.find('"') != string::npos;
}

// Check if the line containes Single quotation mark
bool checkSingleQuotationMark(string& line) {
   return line.find('\'') != string::npos;
}

// Check if the line containes comme
bool checkComma(string& line){
   return line.find(',') != string::npos;
}

// Check if the line containes semicolon
bool checkSemiColon(string& line){
   return line.find(':') != string::npos;
}

// Check if the line containes int 20h
bool checkint20h(string& line){
   return line.find("int 20h") != string::npos;
}

// Check if the line containes brakets
bool checkBrackets(string& line) {
   return line.find('[') != string::npos && line.find(']') != string::npos;
}

// Check if word's specific index is a number
bool isDigitDecimal(string variable,int digit){
   return 48 <= variable.at(digit) && variable.at(digit) <= 57;
}



// Hexadecimal to Decimal
unsigned short hexToDec(std::string hexString) {
   toUpper(hexString);
   int len = hexString.length();
   int base = 1;
   int temp = 0;
   for (int i=len-1; i>=0; i--) {
      if (hexString.at(i)>='0' && hexString.at(i)<='9') {
         temp += (hexString.at(i) - 48)*base;
         base = base * 16;
      }
      else if (hexString.at(i)>='A' && hexString.at(i)<='F') {
         temp += (hexString.at(i) - 55)*base;
         base = base*16;
      }
   }
   return temp;
}

// Decimal to Hexadecimal
string decToHex(int n) { 
    string result = "";    
    if(n == 0) return "0";
    // char array to store hexadecimal number 
      
    // counter for hexadecimal number array 
    while(n!=0) {
        // temporary variable to store remainder 
        int temp  = 0; 
        // storing remainder in temp variable. 
        temp = n % 16; 
        // check if temp < 10 
        if(temp < 10) {
            result = (char)(temp + 48) + result; 
        } 
        else {
            result = (char)(temp + 55) + result; 
        } 
          
        n = n/16; 
    } 
    return result;
    // printing hexadecimal number array in reverse order 
   
    
} 

// Decimal to Binary
template <class datatype>
string decToBin(datatype x) {
    string bits = "";
    for(int i=8*sizeof(x)-1; i>=0; i--) {
      (x & (1 << i)) ? bits += '1' : bits += '0';
    }
    return bits;
}

// Binary to Decimal
int binToDec(string number) {
    int n = 0,index = 0;
    for (int i = number.length()-1; i >= 0; i--)
    {
       n += (number.at(i) - '0') * pow(2,index++);
    }
    return n;
}

// Binary to Decimal
int binToDec(long long int n) {
    int decimalNumber = 0, i = 0, remainder;
    while (n!=0)
    {
        remainder = n%10;
        n /= 10;
        decimalNumber += remainder*pow(2,i);
        ++i;
    }
    return decimalNumber;
}

// Prints bits of the parameter
template <class datatype> 
void printBits(datatype x) {
    int i;

    for(i=8*sizeof(x)-1; i>=0; i--) {
      (x & (1 << i)) ? putchar('1') : putchar('0');
    }
    printf("\n");
}

// Prints hexadecimal value of the parameter
template <class datatype> 
void print_hex(datatype x) {
   if (sizeof(x) == 1) 
      printf("%02x\n",x); 
   else 
      printf("%04x\n",x); 
}

// Prints all sixteen bit and eight bit registers 
void print_16bitregs() {
   printf("AX:%04x\n",ax); 
   printf("BX:%04x\n",bx); 
   printf("CX:%04x\n",cx); 
   printf("DX:%04x\n",dx); 
   printf("AH:%04x\n",*pah); 
   printf("AL:%04x\n",*pal); 
   printf("BH:%04x\n",*pbh); 
   printf("BL:%04x\n",*pbl); 
   printf("CH:%04x\n",*pch); 
   printf("CL:%04x\n",*pcl); 
   printf("DH:%04x\n",*pdh); 
   printf("DL:%04x\n",*pdl); 
   printf("SP:%04x\n",sp); 
}
