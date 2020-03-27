#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include<sstream>
#include <algorithm>
#include<stdlib.h>
#include<math.h>
// prototypes 
template <class datatype> void printBits(datatype x);
template <class datatype> void print_hex(datatype x) ; 
template <class regtype>  void mov_reg_reg(regtype *preg1,regtype *preg2)  ;
void print_16bitregs() ; 

// global variables ( memory, registers and flags ) 
unsigned char memory[2<<15] ;    // 64K memory 
unsigned short ax = 0 ; 
unsigned short bx = 0 ;
unsigned short cx = 0 ; 
unsigned short dx = 0 ;

// unsigned short di = 0 ; 
// unsigned short si = 0 ; 
// unsigned short bp = 0 ; 

unsigned short PC = 0 ; 
// unsigned short sp = 0 ;
unsigned short sp = (2<<15)-2 ;



bool zf = 0;              // zero flag
bool cf = 0;              // carry flag
bool af = 0;              // auxillary flag 
bool sf = 0;              // sign flag 
bool of = 0;              // overflow flag


// INSTRUCTIONS 

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



// initialize pointers 
unsigned short *pax = &ax ; 
unsigned short *pbx = &bx ; 
unsigned short *pcx = &cx ; 
unsigned short *pdx = &dx ; 


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

struct dbVariable{
    string name;
    int address = 0;
    unsigned char value = 0;
};


struct dwVariable{
    string name;
    int address = 0;
    unsigned short value = 0;
};

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
vector<string> afterCompare;
bool isJustAfterCompare = false;
bool lineWithoutLabel = false;
int numberOfLinesWithoutVariables = 0;
// Pair<Pair<nameOfVariable,Pair<typeOfVariable,balueOfVariable>>,addressOfVariable>
// nameOfVariable - typeOfVariable
vector<pair<string,string>> queueOfVariables;


// FUNCTIONS

// **********************************************
//    C H E C K S

bool checkSpace(string& line);
bool checkQuotationMarks(string& line);
bool checkSingleQuotationMark(string& line);
bool checkComma(string& line);
bool checkSemiColon(string& line);
bool checkint20h(string& line);
bool isDigitDecimal(string variable,int digit);
bool checkBrackets(string& line);


// GET VALUE OF SOMETHING
int determineValueOfInstruction(string reg);
int getOtherValue(string str1);
int getVariableAddress(string& variable);
int getVariableValueFromMemoryAddress(string address);
string getVariableNameFromVariableAddress(string address);
int getIndexOfLabel(string & labelName);
void determineReg(unsigned short **pmx, unsigned char **pmhl, string& reg,bool& isVariableFound1,bool& isVariableFound2,std::vector<dbVariable>::iterator &it,std::vector<dwVariable>::iterator &it2);


// SET VALUE OF SOMETHING
void setVariableAddress(string& variable,int address);
void setVariableValue(string variableName,int value);
void checkAndSetFlags(unsigned short number1,unsigned short number2,int bit,string option);


// CLEANING AND PARSING
string cleanVariable(string variable);
inline std::string trim(std::string& str);
void getLineTrimLower(ifstream& inFile,string& firstLine);
inline string getLinestreamLine(istringstream& linestream,string& word,char option);

void twoWordsComma(istringstream& linestream,string& secondWord, string& thirdWord);
void thirdWordsComma(istringstream& linestream,string& secondWord, string& thirdWord, string& forthWord);


void parseInput(string& line,ifstream& inFile);
void createLabel(ifstream& inFile,string& labelName);
void createLinesWithoutLabels(ifstream& inFile,string& stringName);
void createDbOrDw(string& line);
void getLabelContent(Label& label,ifstream& inFile,string& line);
void determineLabelVariables();
void constructMemory();
void processLabels(int index);
void processTwoWordsInstructions(string& option, string& str1,string& str2,string& str3);
void processOneWordInstructions(string& option, string& str1);





//UTILITY FUNCTIONS
unsigned short hexToDec(std::string hexString);
string decToHex(int n);
template <class datatype> string decToBin(datatype x);
int binToDec(string number);
int binToDec(long long int n);
void toUpper(string& firstLine);
void toLower(string& firstLine);
int countSpecificCharacter(string str, char character);
void printLabels();
void printVariables();




// INSTRUCTIONS
void instructionOptions(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,std::vector<dbVariable>::iterator firstIt,std::vector<dwVariable>::iterator firstIt2,bool& isFirstVariableFound1,bool& isFirstVariableFound2,string& str1,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl,string option);
template <class regOne, class regTwo>  void moveValueToVariable(regOne& firstIt,regTwo *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,string& str2,string& str3,string option);
template <class regOne, class regTwo>  void moveValueToReg(regOne** firstReg, regTwo* secondReg,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,string& str2,string& str3,string option);
void instructionForBrakets(string str1,string str2,string str3,string option);
void add(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl);
void sub(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl);
void cmp(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl);
void comparison(unsigned short firstValue,unsigned short secondValue);


int main() {

    // Open the input and output files, check for failures
    ifstream inFile("atwon.txt");
    if (!inFile) { // operator! is synonymous to .fail(), checking if there was a failure
        cerr << "There was a problem opening \"" << "atwon.txt" << "\" as input file" << endl;
        return 1;
    }

   string firstLine;
   getLineTrimLower(inFile,firstLine);

   while(!checkint20h(firstLine) && !inFile.eof()){
      parseInput(firstLine,inFile);  
      getLineTrimLower(inFile,firstLine);
   }
   determineLabelVariables();
   constructMemory();
   processLabels(0);
}

// INPUT PARSING
void parseInput(string& line,ifstream& inFile){
   if(line.find("code segment") != string::npos || line.find("code ends") != string::npos) return;
   bool isDb = line.find("db") != string::npos;
   bool isDw = line.find("dw") != string::npos;
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
   label.name = labelName;
   getLabelContent(label,inFile,stringName);
   
}

// CREATE LABEL WITHOUT NAMES
void createLinesWithoutLabels(ifstream& inFile,string& stringName){
   string labelName = "";
   Label label;
   label.name = "Without Labels";
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
      

      if(secondWord == "db"){
         dbVariable variable;
         variable.name = firstWord;
         if(checkQuotationMarks(thirdWord) || checkSingleQuotationMark(thirdWord)){
            variable.value = (unsigned short)thirdWord.at(1);
         }else{
            variable.value = stoi(thirdWord);
         }
         dbVariables.push_back(variable);
         queueOfVariables.push_back(make_pair(variable.name,"db"));
      }else{
         dwVariable variable;
         variable.name = firstWord;
         if(checkQuotationMarks(thirdWord) || checkSingleQuotationMark(thirdWord)){
            variable.value = (unsigned short)thirdWord.at(1);
         }else{
            variable.value = stoi(thirdWord);
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
      if(labels.at(i).name != "Without Labels") numberOfLinesWithoutVariables += 1;
   for (it = labels.at(i).content.begin(); it != labels.at(i).content.end(); it++) {
      line = *it;
      if(line.find("int 20h") != string::npos) return;
      }
   }
}



// CONSTRUCT MEMORY
void constructMemory(){
   cout << sizeof(memory)/sizeof(*memory) << endl;
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
            memory[address] = (*it).value;
            isVariableFound1 = true;
            break;
         }

      }
      if(!isVariableFound1){
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
      {
         if(variable == (*it2).name){
            (*it2).address = address;
            memory[address] = (*it2).value;
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
   bool newJmpFound = false;
   string newLabel = "";
   for (int i = index; i < labels.size() && !newJmpFound; i++) {
   for (it = labels.at(i).content.begin(); it != labels.at(i).content.end() && !newJmpFound; it++) {
   string temp,firstWord,secondWord,thirdWord,forthWord,fifthWord,sixthWord,seventhWord = "";

      print_16bitregs();
      line = (*it);
      istringstream linestream(line);
      getLinestreamLine(linestream,firstWord,' ');
      if(firstWord.at(0) != 'j'){
         isJustAfterCompare = false;
         afterCompare.clear();
      }  
      
      if(firstWord == "mov" || firstWord == "add" || firstWord == "sub" || firstWord == "xor" || firstWord == "or" || firstWord == "and" || firstWord == "not" || firstWord == "shr" || firstWord == "shl" || firstWord == "rcl" || firstWord == "rcr"){
         if(line.find("offset") != string::npos) thirdWordsComma(linestream,secondWord,thirdWord,forthWord);
         else twoWordsComma(linestream,secondWord,thirdWord);
         processTwoWordsInstructions(firstWord,secondWord,thirdWord,forthWord);
      }else if(firstWord == "int" || firstWord == "mul" || firstWord == "div" || firstWord == "push" || firstWord == "pop"){
         getLinestreamLine(linestream,secondWord,' ');
         processOneWordInstructions(firstWord,secondWord);
      }else if(firstWord == "nop"){
         
      }else if(firstWord == "cmp"){
         twoWordsComma(linestream,secondWord,thirdWord);
         processTwoWordsInstructions(firstWord,secondWord,thirdWord,forthWord);
         isJustAfterCompare = true;
      }else if(firstWord.at(0) == 'j'){
         getLinestreamLine(linestream,secondWord,' ');
         secondWord = secondWord.substr(0,secondWord.length()-1);

         if(firstWord == "jmp"){
            newJmpFound = true;
            newLabel = secondWord;
            index2 = getIndexOfLabel(newLabel);
         }else{
         vector<string>::iterator it;
         for (it = afterCompare.begin(); it != afterCompare.end(); it++) {
            if((*it) == firstWord){
               newJmpFound = true;
               newLabel = secondWord;
               index2 = getIndexOfLabel(newLabel);
               break;
            }

         }
         }
         
      }
   }
   }

   if(index2 != -1) processLabels(index2);
}

// PROCESS TWO WORDS INSTRUCTIONS
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
      unsigned char eightBit = 0;
   // For Register Names

      determineReg(&pmx,&pmhl,str1,isFirstVariableFound1,isFirstVariableFound2,firstIt,firstIt2);
      if(str3 != "offset")
      determineReg(&pnx,&pnhl,str2,isVariableFound1,isVariableFound2,it,it2);
      

      if(option == "mov" || option == "add" || option == "sub" || option == "or" || option == "and" || option == "xor" || option == "not" || option == "rcl" || option == "rcr" || option == "shr" || option == "shl"){
         instructionOptions(pmx,pnx,it,it2,firstIt,firstIt2,isFirstVariableFound1,isFirstVariableFound2,str1,str2,str3,pmhl,pnhl,option);
      }else if(option == "cmp"){
         cmp(pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3,pmhl,pnhl);
      }


}

// PROCESS ONE WORD INSTRUCTIONS
void processOneWordInstructions(string& option, string& str1){
   if(option == "int" && str1 == "21h"){
      if(*pah == 1){
         char temp;
         cin >> temp;
         *pal = (unsigned char)temp;
         cout << (char)(*pal);
      }else if(decToHex(*pah) == "2"){
         cout << (char)(*pdl) ;
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
      print_16bitregs();
      exit(0);
   }else if(option == "div"){
      *pax /= (unsigned short)determineValueOfInstruction(str1);
   }else if(option == "mul"){
      checkAndSetFlags(*pax,determineValueOfInstruction(str1),16,option);
      *pax *= (unsigned short)determineValueOfInstruction(str1);
   }else if(option == "push"){
      unsigned short deger = (unsigned short)determineValueOfInstruction(str1);
      sp = sp << 8;
      sp = sp | deger;
      print_16bitregs() ; 

   }else if(option == "pop"){
      unsigned short *pmx = nullptr; 
      unsigned char *pmhl = nullptr;
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      determineReg(&pmx,&pmhl,str1,isVariableFound1,isVariableFound2,it,it2);
      unsigned short temp;
      temp = sp >> 8;
      temp = temp << 8;
      if(pmx != nullptr) *pmx = (sp - temp);
      else if(pmhl != nullptr) *pmhl = (sp - temp);
      else if(isVariableFound1) (*it).value = (sp - temp);
      else if(isVariableFound2) (*it2).value = (sp - temp);
      sp = sp >> 8;
      print_16bitregs() ; 

      
   }

}

// DETERMINE : pmx,pmhl,it,it2,isVariableFound1,isVariableFound2
void determineReg(unsigned short **pmx, unsigned char **pmhl, string& reg,bool& isVariableFound1,bool& isVariableFound2,std::vector<dbVariable>::iterator &it,std::vector<dwVariable>::iterator &it2) {
   string resultReg = cleanVariable(reg);
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
   }


      for (it = dbVariables.begin(); it != dbVariables.end(); it++) {
         if(resultReg == (*it).name){
            isVariableFound1 = true;
            break;
         }

      }
      if(!isVariableFound1){
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
      {
         if(resultReg == (*it2).name){
            isVariableFound2 = true;
            break;
         }
      }
      }



}

// DETERMINE VALUE OF INSTRUCTION
// ax,al,msg,1,1d,1h,'1',"1",[msg],[ax],[0090h],[0090]
int determineValueOfInstruction(string reg) {
   string resultReg= "";
   resultReg = cleanVariable(reg);
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
   }else{
      for (it = dbVariables.begin(); it != dbVariables.end(); it++)
      {
         if(resultReg == (*it).name){
            isVariableFound1 = true;
            break;
         }

      }
      if(!isVariableFound1){
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
      {
         if(resultReg == (*it2).name){
            isVariableFound2 = true;
            break;
         }
      }
      }

    if(isVariableFound1){
         result = (unsigned char)((*it).value); 
      }else if(isVariableFound2){
            result = (unsigned short)((*it2).value); 
      }else{
         result = getOtherValue(reg);
      }
      
   }
   return result;
}

// determineValueOfInstruction Calls This Function -> getOtherValue
// 1,1d,1h,'1',"1",[0090h],[0090]
int getOtherValue(string str1) {
   int result = 0;
   char character;

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
      }else{
         result = stoi(str1);
      }
      
   }else if((str1.at(0) == '[' && str1.at(str1.length()-1) == ']') && isDigitDecimal(str1,1)){
      result = memory[stoi(cleanVariable(str1))];
   }else{
      
      if(str1.at(0) == '\''){
         character = str1.at(1);
      }else if(str1.at(0) == '"' || str1.at(0) == '\\' || str1.at(0) == '\"'){
         character = str1.at(1);
      }else{
         character = str1.at(0);
      }
      result = character;
   }
      return result;
}

//     M O V E    F U N C T I O N
void instructionOptions(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,std::vector<dbVariable>::iterator firstIt,std::vector<dwVariable>::iterator firstIt2,bool& isFirstVariableFound1,bool& isFirstVariableFound2,string& str1,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl,string option) {
         char character;
         if(pmx != nullptr){
            moveValueToReg(&pmx,pnx,it,it2,str2,str3,option);
         }else if(pmhl != nullptr){
            moveValueToReg(&pmhl,pnhl,it,it2,str2,str3,option);
         }else if(isFirstVariableFound1){
            if(pnhl != nullptr) moveValueToVariable(firstIt,pnhl,it,it2,str2,str3,option);
            else moveValueToVariable(firstIt,pnx,it,it2,str2,str3,option);
         }else if(isFirstVariableFound2){
            if(pnhl != nullptr) moveValueToVariable(firstIt2,pnhl,it,it2,str2,str3,option);
            else moveValueToVariable(firstIt2,pnx,it,it2,str2,str3,option);
         }else if(str1.at(0) == '[' && str1.at(str1.length()-1) == ']' && isDigitDecimal(str1,1)){
            instructionForBrakets(str1,str2,str3,option);
      }  
}

void cmp(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl){
         char character;
         if(pmx != nullptr){
            if(pnx != nullptr){
               comparison(*pmx,*pnx);
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1 || isVariableFound2){
               comparison(*pmx,determineValueOfInstruction(str2));
            }else{
               comparison(*pmx,getOtherValue(str2));
            }
         }else if(pmhl != nullptr){
            if(pnhl != nullptr){
               comparison(*pmhl,*pnhl);
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1 || isVariableFound2){
              comparison(*pmhl,determineValueOfInstruction(str2));
            }else{
              comparison(*pmhl,getOtherValue(str2));
            }
         }
}





template <class regOne, class regTwo> 
void moveValueToReg(regOne** firstReg, regTwo* secondReg,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,string& str2,string& str3,string option){
   if(option == "mov"){
      if(secondReg != nullptr){
         **firstReg = *secondReg;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         **firstReg = getVariableAddress(str2);
      }else{
         **firstReg = determineValueOfInstruction(str2);
      }
   }else if(option == "add"){
      if(secondReg != nullptr){
         checkAndSetFlags(**firstReg,*secondReg,sizeof(**firstReg)*8,"add");
         **firstReg += *secondReg;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"add");
         **firstReg += getVariableAddress(str2);
      }else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"add");
         **firstReg += resultOfInstruction;
      }
   }else if(option == "sub"){
      if(secondReg != nullptr){
         checkAndSetFlags(**firstReg,*secondReg,sizeof(**firstReg)*8,"sub");
         **firstReg -= *secondReg;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"sub");
         **firstReg -= getVariableAddress(str2);
      }else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"sub");
         **firstReg -= determineValueOfInstruction(str2);
      }
   }else if(option == "or"){
      if(secondReg != nullptr){
         checkAndSetFlags(**firstReg,*secondReg,sizeof(**firstReg)*8,"or");
         **firstReg |= *secondReg;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"or");
         **firstReg |= getVariableAddress(str2);
      }else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"or");
         **firstReg |= resultOfInstruction;
      }
   }else if(option == "and"){
      if(secondReg != nullptr){
         checkAndSetFlags(**firstReg,*secondReg,sizeof(**firstReg)*8,"and");
         **firstReg &= *secondReg;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"and");
         **firstReg &= getVariableAddress(str2);
      }else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"and");
         **firstReg &= resultOfInstruction;
      }
   }else if(option == "not"){
      **firstReg = ~(**firstReg);
   }else if(option == "xor"){
      if(secondReg != nullptr){
         checkAndSetFlags(**firstReg,*secondReg,sizeof(**firstReg)*8,"xor");
         **firstReg ^= *secondReg;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"xor");
         **firstReg ^= getVariableAddress(str2);
      }else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"xor");
         **firstReg ^= resultOfInstruction;
      }
   }else if(option == "shr"){
      if(secondReg != nullptr){
         checkAndSetFlags(**firstReg,*secondReg,sizeof(**firstReg)*8,"shr");
         for (int i = 0; i < *secondReg; i++) {
            cf = **firstReg & 1;
            **firstReg >>= 1;
         }
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"shr");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = **firstReg & 1;
            **firstReg >>= 1;
         }
      }else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"shr");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = **firstReg & 1;
            **firstReg >>= 1;
            printBits(ax);
         }
      }
   }else if(option == "shl"){
      if(secondReg != nullptr){
         checkAndSetFlags(**firstReg,*secondReg,sizeof(**firstReg)*8,"shl");
         for (int i = 0; i < *secondReg; i++) {
            cf = decToBin(**firstReg).at(0)-'0';
            **firstReg <<= 1;
         }
         
        
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"shl");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = decToBin(**firstReg).at(0)-'0';
            **firstReg <<= 1;
         }
      }else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"shl");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = decToBin(**firstReg).at(0)-'0';
            **firstReg <<= 1;
            printBits(ax);
         }
      }
   }else if(option == "rcr"){
      if(secondReg != nullptr){
         checkAndSetFlags(**firstReg,*secondReg,sizeof(**firstReg)*8,"rcr");
         for (int i = 0; i < *secondReg; i++) {
            cf = **firstReg & 1;
            unsigned short temp = cf << sizeof(**firstReg) * 8;
            **firstReg >>= 1;
            **firstReg |= temp;
         }
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"rcr");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = **firstReg & 1;
            unsigned short temp = cf << sizeof(**firstReg) * 8;
            **firstReg >>= 1;
            **firstReg |= temp;
         }
      }else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"rcr");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = **firstReg & 1;
            unsigned short temp = cf << sizeof(**firstReg) * 8 - 1;
            **firstReg >>= 1;
            **firstReg |= temp;
         }
      }
   }else if(option == "rcl"){
      if(secondReg != nullptr){
         checkAndSetFlags(**firstReg,*secondReg,sizeof(**firstReg)*8,"rcl");
         for (int i = 0; i < *secondReg; i++) {
            cf = decToBin(**firstReg).at(0)-'0';
            **firstReg <<= 1;
            **firstReg |= cf;
         }
         
        
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"rcl");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = decToBin(**firstReg).at(0)-'0';
            **firstReg <<= 1;
            **firstReg |= cf;
         }
      }else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags(**firstReg,resultOfInstruction,sizeof(**firstReg)*8,"rcl");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = decToBin(**firstReg).at(0)-'0';
            **firstReg <<= 1;
            **firstReg |= cf;
            printBits(ax);
         }
      }
   }
}

template <class regOne, class regTwo> 
void moveValueToVariable(regOne& firstIt,regTwo *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,string& str2,string& str3,string option){
   if(option == "mov"){
      if(pnx != nullptr){
         (*firstIt).value = *pnx;
         memory[(*firstIt).address] = (*firstIt).value;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         (*firstIt).value = getVariableAddress(str2);
         memory[(*firstIt).address] = (*firstIt).value;
      }
      else{
         (*firstIt).value = determineValueOfInstruction(str2);
         memory[(*firstIt).address] = (*firstIt).value;
      }
   }else if(option == "add"){
      if(pnx != nullptr){
         checkAndSetFlags((*firstIt).value,*pnx,sizeof((*firstIt).value)*8,"add");
         (*firstIt).value += *pnx;
         memory[(*firstIt).address] = (*firstIt).value;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"add");
         (*firstIt).value += resultOfInstruction;
         memory[(*firstIt).address] = (*firstIt).value;
      }
      else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"add");
         (*firstIt).value += resultOfInstruction;
         memory[(*firstIt).address] = (*firstIt).value;
      }
   }else if(option == "sub"){
      if(pnx != nullptr){
         checkAndSetFlags((*firstIt).value,*pnx,sizeof((*firstIt).value)*8,"sub");
         (*firstIt).value -= *pnx;
         memory[(*firstIt).address] = (*firstIt).value;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"sub");
         (*firstIt).value -= resultOfInstruction;
         memory[(*firstIt).address] = (*firstIt).value;
      }
      else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"sub");
         (*firstIt).value -= resultOfInstruction;
         memory[(*firstIt).address] = (*firstIt).value;
      }
   }else if(option == "or"){
      if(pnx != nullptr){
         checkAndSetFlags((*firstIt).value,*pnx,sizeof((*firstIt).value)*8,"or");
         (*firstIt).value |= *pnx;
         memory[(*firstIt).address] = (*firstIt).value;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"or");
         (*firstIt).value |= resultOfInstruction;
         memory[(*firstIt).address] = (*firstIt).value;
      }
      else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"or");
         (*firstIt).value |= resultOfInstruction;
         memory[(*firstIt).address] = (*firstIt).value;
      }
   }else if(option == "and"){
      if(pnx != nullptr){
         checkAndSetFlags((*firstIt).value,*pnx,sizeof((*firstIt).value)*8,"and");
         (*firstIt).value &= *pnx;
         memory[(*firstIt).address] = (*firstIt).value;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"and");
         (*firstIt).value &= resultOfInstruction;
         memory[(*firstIt).address] = (*firstIt).value;
      }
      else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"and");
         (*firstIt).value &= resultOfInstruction;
         memory[(*firstIt).address] = (*firstIt).value;
      }
   }else if(option == "not"){
      (*firstIt).value = ~(*firstIt).value;
      memory[(*firstIt).address] = (*firstIt).value;
   }else if(option == "xor"){
      if(pnx != nullptr){
         checkAndSetFlags((*firstIt).value,*pnx,sizeof((*firstIt).value)*8,"xor");
         (*firstIt).value ^= *pnx;
         memory[(*firstIt).address] = (*firstIt).value;
      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"xor");
         (*firstIt).value ^= resultOfInstruction;
         memory[(*firstIt).address] = (*firstIt).value;
      }
      else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"xor");
         (*firstIt).value ^= resultOfInstruction;
         memory[(*firstIt).address] = (*firstIt).value;
      }
   }else if(option == "shr"){
      if(pnx != nullptr){
         checkAndSetFlags((*firstIt).value,*pnx,sizeof((*firstIt).value)*8,"shr");
         for (int i = 0; i < *pnx; i++) {
            cf = (*firstIt).value & 1;
            (*firstIt).value >>= 1;
         }
         memory[(*firstIt).address] = (*firstIt).value;


      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"shr");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = (*firstIt).value & 1;
            (*firstIt).value >>= 1;
         }
         memory[(*firstIt).address] = (*firstIt).value;
      }
      else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"shr");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = (*firstIt).value & 1;
            (*firstIt).value >>= 1;
         }
         memory[(*firstIt).address] = (*firstIt).value;
      }
   }else if(option == "shl"){
      if(pnx != nullptr){
         checkAndSetFlags((*firstIt).value,*pnx,sizeof((*firstIt).value)*8,"shl");
         for (int i = 0; i < *pnx; i++) {
            cf = decToBin((*firstIt).value).at(0)-'0';
            (*firstIt).value <<= 1;
         }  
         memory[(*firstIt).address] = (*firstIt).value;

      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"shl");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = decToBin((*firstIt).value).at(0)-'0';
            (*firstIt).value <<= 1;
         }
         memory[(*firstIt).address] = (*firstIt).value;


      }
      else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"shl");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = decToBin((*firstIt).value).at(0)-'0';
            (*firstIt).value <<= 1;
         }
         memory[(*firstIt).address] = (*firstIt).value;
      }
   }else if(option == "rcr"){
      if(pnx != nullptr){
         checkAndSetFlags((*firstIt).value,*pnx,sizeof((*firstIt).value)*8,"rcr");
         for (int i = 0; i < *pnx; i++) {
            cf = (*firstIt).value & 1;
            unsigned short temp = cf << sizeof((*firstIt).value) * 8;
            (*firstIt).value >>= 1;
            (*firstIt).value |= temp;
         } 
         memory[(*firstIt).address] = (*firstIt).value;

      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"rcr");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = (*firstIt).value & 1;
            unsigned short temp = cf << sizeof((*firstIt).value) * 8;
            (*firstIt).value >>= 1;
            (*firstIt).value |= temp;
         }
         memory[(*firstIt).address] = (*firstIt).value;


      }
      else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"rcr");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = (*firstIt).value & 1;
            unsigned short temp = cf << sizeof((*firstIt).value) * 8 - 1;
            (*firstIt).value >>= 1;
            (*firstIt).value |= temp;
         }
         memory[(*firstIt).address] = (*firstIt).value;
      }
   }else if(option == "rcl"){
      if(pnx != nullptr){
         checkAndSetFlags((*firstIt).value,*pnx,sizeof((*firstIt).value)*8,"rcl");
         for (int i = 0; i < *pnx; i++) {
            cf = decToBin((*firstIt).value).at(0)-'0';
            (*firstIt).value <<= 1;
            (*firstIt).value |= cf;
         }
         memory[(*firstIt).address] = (*firstIt).value;

      }
         // VARIABLE WILL BE MOVED.
      else if(str3 == "offset"){
         int resultOfInstruction = 0;
         resultOfInstruction = getVariableAddress(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"rcl");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = decToBin((*firstIt).value).at(0)-'0';
            (*firstIt).value <<= 1;
            (*firstIt).value |= cf;
         }
         memory[(*firstIt).address] = (*firstIt).value;


      }
      else{
         int resultOfInstruction = 0;
         resultOfInstruction = determineValueOfInstruction(str2);
         checkAndSetFlags((*firstIt).value,resultOfInstruction,sizeof((*firstIt).value)*8,"rcl");
         for (int i = 0; i < resultOfInstruction; i++) {
            cf = decToBin((*firstIt).value).at(0)-'0';
            (*firstIt).value <<= 1;
            (*firstIt).value |= cf;
            printBits(ax);
         }
         memory[(*firstIt).address] = (*firstIt).value;
      }
   }

}


void instructionForBrakets(string str1,string str2,string str3,string option) {
   int result = 0;
   result = (str3 == "offset") ? getVariableAddress(str2) : determineValueOfInstruction(str2);

   if(option == "mov"){
      memory[stoi(cleanVariable(str1))] = result;
      setVariableValue(getVariableNameFromVariableAddress(str1),result);
   }else if(option == "add"){
      checkAndSetFlags(memory[stoi(cleanVariable(str1))],result,sizeof(memory[stoi(cleanVariable(str1))])*8,"add");
      memory[stoi(cleanVariable(str1))] += result;
      setVariableValue(getVariableNameFromVariableAddress(str1),memory[stoi(cleanVariable(str1))]);
   }else if(option == "sub"){
      checkAndSetFlags(memory[stoi(cleanVariable(str1))],result,sizeof(memory[stoi(cleanVariable(str1))])*8,"sub");
      memory[stoi(cleanVariable(str1))] -= result;
      setVariableValue(getVariableNameFromVariableAddress(str1),memory[stoi(cleanVariable(str1))]);
   }else if(option == "or"){
      checkAndSetFlags(memory[stoi(cleanVariable(str1))],result,sizeof(memory[stoi(cleanVariable(str1))])*8,"or");
      memory[stoi(cleanVariable(str1))] |= result;
      setVariableValue(getVariableNameFromVariableAddress(str1),memory[stoi(cleanVariable(str1))]);
   }else if(option == "and"){
      checkAndSetFlags(memory[stoi(cleanVariable(str1))],result,sizeof(memory[stoi(cleanVariable(str1))])*8,"and");
      memory[stoi(cleanVariable(str1))] &= result;
      setVariableValue(getVariableNameFromVariableAddress(str1),memory[stoi(cleanVariable(str1))]);
   }else if(option == "not"){
      memory[stoi(cleanVariable(str1))] = ~memory[stoi(cleanVariable(str1))];
      setVariableValue(getVariableNameFromVariableAddress(str1),memory[stoi(cleanVariable(str1))]);
   }else if(option == "xor"){
      checkAndSetFlags(memory[stoi(cleanVariable(str1))],result,sizeof(memory[stoi(cleanVariable(str1))])*8,"xor");
      memory[stoi(cleanVariable(str1))] ^= result;
      setVariableValue(getVariableNameFromVariableAddress(str1),memory[stoi(cleanVariable(str1))]);
   }else if(option == "shr"){
      checkAndSetFlags(memory[stoi(cleanVariable(str1))],result,sizeof(memory[stoi(cleanVariable(str1))])*8,"shr");
      for (int i = 0; i < result; i++) {
         cf = memory[stoi(cleanVariable(str1))] & 1;
         memory[stoi(cleanVariable(str1))] >>= 1;
      }
      setVariableValue(getVariableNameFromVariableAddress(str1),memory[stoi(cleanVariable(str1))]);
   }else if(option == "shl"){
      checkAndSetFlags(memory[stoi(cleanVariable(str1))],result,sizeof(memory[stoi(cleanVariable(str1))])*8,"shl");
      for (int i = 0; i < result; i++) {
            cf = decToBin(memory[stoi(cleanVariable(str1))]).at(0)-'0';
            memory[stoi(cleanVariable(str1))] <<= 1;
         } 
      setVariableValue(getVariableNameFromVariableAddress(str1),memory[stoi(cleanVariable(str1))]);

   }else if(option == "rcr"){
      checkAndSetFlags(memory[stoi(cleanVariable(str1))],result,sizeof(memory[stoi(cleanVariable(str1))])*8,"rcr");
      for (int i = 0; i < result; i++) {
         cf = memory[stoi(cleanVariable(str1))] & 1;
         unsigned short temp = cf << sizeof(memory[stoi(cleanVariable(str1))]) * 8;
         memory[stoi(cleanVariable(str1))] >>= 1;
         memory[stoi(cleanVariable(str1))] |= temp;
      } 
      setVariableValue(getVariableNameFromVariableAddress(str1),memory[stoi(cleanVariable(str1))]);
   }else if(option == "rcl"){
      checkAndSetFlags(memory[stoi(cleanVariable(str1))],result,sizeof(memory[stoi(cleanVariable(str1))])*8,"rcl");
      for (int i = 0; i < result; i++) {
            cf = decToBin(memory[stoi(cleanVariable(str1))]).at(0)-'0';
            memory[stoi(cleanVariable(str1))] <<= 1;
            memory[stoi(cleanVariable(str1))] |= cf;
         }
      setVariableValue(getVariableNameFromVariableAddress(str1),memory[stoi(cleanVariable(str1))]);
   }

}

void comparison(unsigned short firstValue,unsigned short secondValue){
   if(firstValue == secondValue && zf == 1) {
      afterCompare.push_back("je");
      afterCompare.push_back("jz");
   }
   if(firstValue != secondValue && zf == 0) {
      afterCompare.push_back("jne");
      afterCompare.push_back("jnz");
   }
   if(firstValue > secondValue && cf == 0 && zf == 0) {
      afterCompare.push_back("ja");
      afterCompare.push_back("jnbe");
   }
   if(firstValue < secondValue && cf == 1) {
      afterCompare.push_back("jb");
      afterCompare.push_back("jnae");
      afterCompare.push_back("jc");
   }
   if(firstValue >= secondValue && cf == 0) {
      afterCompare.push_back("jnb");
      afterCompare.push_back("jae");
      afterCompare.push_back("jnc");
   }
   if(firstValue <= secondValue && cf == 1 && zf == 1) {
      afterCompare.push_back("jbe");
      afterCompare.push_back("jna");
   }

}


// FLAGS

void checkAndSetFlags(unsigned short number1,unsigned short number2,int bit,string option) {
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
      }

   }else if(option == "sub"){
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
      }
   }else if(option == "mul"){
      if(bit == 8){
         eightBitResult = num1EightBit * number2;
         if(num1EightBit * number2 > 255) cf = 1;
         else cf = 0;
         // if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         // else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         // else of = 0;

      }else if(bit == 16){
         sixteenBitResult = num1SixteenBit * number2;
         if(num1SixteenBit * number2 > 65535) cf = 1;
         else cf = 0;
         // if(!isFirstDigitOne1 && !isFirstDigitOne2 && sf == 1) of = 1;
         // else if(isFirstDigitOne1 && isFirstDigitOne2 && sf == 0) of = 1;
         // else of = 0;
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
      }
   }else if(option == "rcl"){
      
   }else if(option == "rcr"){
      
   }else if(option == "shl"){
      
   }else if(option == "shr"){
      
   }else if(option == "cmp"){
      
   }



}













// FUNCTIONS ABOUT MEMORY ADDRESS
void setVariableValue(string variableName,int value){
   variableName = cleanVariable(variableName);
   std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      for (it = dbVariables.begin(); it != dbVariables.end(); it++) {
         if(variableName == (*it).name){
            (*it).value = value;
            isVariableFound1 = true;
            break;
         }

      }
      if(!isVariableFound1){
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++) {
         if(variableName == (*it2).name){
            (*it).value = value;
            isVariableFound2 = true;
            break;
         }
      }
      }
}



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
int getVariableValueFromMemoryAddress(string address) {
   return memory[stoi(cleanVariable(address))];
}

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







//REMOVE [,],h  FROM WORDS 
string cleanVariable(string variable) {
   if(variable.find('[') != string::npos && variable.find(']') != string::npos){
      variable = variable.substr(variable.find_first_of('[')+1,variable.length());
      variable = variable.substr(0,variable.find_last_of(']'));
      if((variable.at(variable.length()-1) == 'h' || variable.at(variable.length()-1) == 'd') && variable.at(0) >= 48 && variable.at(0) <= 57) variable = variable.substr(0,variable.length()-1);
   } 
   return variable;
}

// SEPARATE WORDS
void twoWordsComma(istringstream& linestream,string& secondWord, string& thirdWord){
   getLinestreamLine(linestream,secondWord,',');
   getLinestreamLine(linestream,thirdWord,' ');
}

void thirdWordsComma(istringstream& linestream,string& secondWord, string& thirdWord, string& forthWord){
   getLinestreamLine(linestream,secondWord,',');
   getLinestreamLine(linestream,forthWord,' ');
   getLinestreamLine(linestream,thirdWord,' ');
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

// PRINT LABELS
int getIndexOfLabel(string & labelName) {

   std::vector<string>::iterator it;
   
   for (int i = 0; i < labels.size(); i++) {
      if(labels.at(i).name == labelName){
         return i;
         }
      }
      return -1;
}



// TRIMS LINESTREAMS
// TRIM LINES
inline std::string trim(std::string& str) {
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    str.erase(str.find_last_not_of(' ')+1);         //surfixing spaces
    return str;
}

void getLineTrimLower(ifstream& inFile,string& firstLine){
   getline(inFile,firstLine);
   trim(firstLine);
   if(firstLine.find("\r") != string::npos) firstLine = firstLine.substr(0,firstLine.length()-1);
   toLower(firstLine);
   
}

inline string getLinestreamLine(istringstream& linestream,string& word,char option) {
   do{
      getline(linestream,word,option);
      if(!linestream) break;
   }while(word == "");
      trim(word);
    return word;
}

int countSpecificCharacter(string str, char character) {
   int count = 0;
   for (int i = 0; i < str.length(); i++) {
      if(str.at(i) == character) count++;
   }
   return count;
   
}



// LOWER CASE
void toLower(string& firstLine){
   transform(firstLine.begin(), firstLine.end(), firstLine.begin(), ::tolower);
}
void toUpper(string& firstLine){
   transform(firstLine.begin(), firstLine.end(), firstLine.begin(), ::toupper);
}


//       C H E C K S
bool checkSpace(string& line){
   return line.find(' ') != string::npos;
}
bool checkQuotationMarks(string& line){
   return line.find('"') != string::npos;
}
bool checkSingleQuotationMark(string& line) {
   return line.find('\'') != string::npos;
}
bool checkComma(string& line){
   return line.find(',') != string::npos;
}
bool checkSemiColon(string& line){
   return line.find(':') != string::npos;
}
bool checkint20h(string& line){
   return line.find("int 20h") != string::npos;
}
bool checkBrackets(string& line) {
   return line.find('[') != string::npos && line.find(']') != string::npos;
}
bool isDigitDecimal(string variable,int digit){
   return 48 <= variable.at(digit) && variable.at(digit) <= 57;
}
//       E N D   C H E C K S

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

template <class datatype>
string decToBin(datatype x) {
    string bits = "";
    for(int i=8*sizeof(x)-1; i>=0; i--) {
      (x & (1 << i)) ? bits += '1' : bits += '0';
    }
    return bits;
}

int binToDec(string number) {
    int n = 0,index = 0;
    for (int i = number.length()-1; i >= 0; i--)
    {
       n += (number.at(i) - '0') * pow(2,index++);
    }
    return n;
}

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

template <class datatype> 
void printBits(datatype x) {
    int i;

    for(i=8*sizeof(x)-1; i>=0; i--) {
      (x & (1 << i)) ? putchar('1') : putchar('0');
    }
    printf("\n");
}

template <class datatype> 
void print_hex(datatype x) {
   if (sizeof(x) == 1) 
      printf("%02x\n",x); 
   else 
      printf("%04x\n",x); 
}

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


// YEDEK OR
// void orOfValues(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,std::vector<dbVariable>::iterator firstIt,std::vector<dwVariable>::iterator firstIt2,bool& isVariableFound1,bool& isVariableFound2,bool& isFirstVariableFound1,bool& isFirstVariableFound2,string& str2,string& str3,unsigned char *pmhl,unsigned char *pnhl) {

//    char character;
//    if(pmx != nullptr){
//       if(pnx != nullptr){

//       }else if(pnhl != nullptr){

//       }else if(isVariableFound1){

//       }else if(isVariableFound2){

//       }else{

//       }
//    }else if(pmhl != nullptr){
//       if(pnx != nullptr){

//       }else if(pnhl != nullptr){

//       }else if(isVariableFound1){

//       }else if(isVariableFound2){

//       }else{
         
//       }
//    }else if(isFirstVariableFound1){
//       if(pnx != nullptr){

//       }else if(pnhl != nullptr){

//       }else if(isVariableFound1){

//       }else if(isVariableFound2){

//       }else{
         
//       }
//    }else if(isFirstVariableFound2){
//       if(pnx != nullptr){

//       }else if(pnhl != nullptr){

//       }else if(isVariableFound1){

//       }else if(isVariableFound2){

//       }else{
         
//       }
//    }else{
//       if(pnx != nullptr){

//       }else if(pnhl != nullptr){

//       }else if(isVariableFound1){

//       }else if(isVariableFound2){

//       }else{
         
//       }
  
//    }
//    // moveValueToReg(&pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3);
//    // else if(pmhl != nullptr){
//    //    moveValueToReg(&pmhl,pnhl,it,it2,isVariableFound1,isVariableFound2,str2,str3);
//    // }else if(isFirstVariableFound1){
//    //    if(pnhl != nullptr) moveValueToVariable(firstIt,pnhl,it,it2,isVariableFound1,isVariableFound2,str2,str3);
//    //    else moveValueToVariable(firstIt,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3);
//    // }else if(isFirstVariableFound2){
//    //    if(pnhl != nullptr) moveValueToVariable(firstIt2,pnhl,it,it2,isVariableFound1,isVariableFound2,str2,str3);
//    //    else moveValueToVariable(firstIt2,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3);
//    // }
// }