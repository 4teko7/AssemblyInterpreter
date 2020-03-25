#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include<sstream>
#include <algorithm>
#include<stdlib.h>
#include<math.h>
// prototypes 
template <class datatype> void print_bits(datatype x) ; 
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
bool sf = 0;              // sign flag 
bool cf = 0;              // carry flag
bool af = 0;              // auxillary flag 
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
int8_t *pah = (int8_t *) ( ( (int8_t *) &ax) + 1);
int8_t *pal = (int8_t *) &ax;
int8_t *pbh = (int8_t *) ( ( (int8_t *) &bx) + 1);
int8_t *pbl = (int8_t *) &bx;
int8_t *pch = (int8_t *) ( ( (int8_t *) &cx) + 1);
int8_t *pcl = (int8_t *) &cx;
int8_t *pdh = (int8_t *) ( ( (int8_t *) &dx) + 1);
int8_t *pdl = (int8_t *) &dx;

using namespace std;

//   OBJECTS

struct dbVariable{
    string name;
    int address = 0;
    int8_t value = 0;
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
void createDbOrDw(string& line);
bool checkSpace(string& line);
bool checkQuotationMarks(string& line);
bool checkSingleQuotationMark(string& line);
bool checkComma(string& line);
bool checkSemiColon(string& line);
bool checkint20h(string& line);
void getLabelContent(Label& label,ifstream& inFile,string& line);
void createLabel(ifstream& inFile,string& labelName);
void getLineTrimLower(ifstream& inFile,string& firstLine);
void printLabels();
void printVariables();
void twoWordsComma(istringstream& linestream,string& secondWord, string& thirdWord);
void thirdWordsComma(istringstream& linestream,string& secondWord, string& thirdWord, string& forthWord);
void processTwoWordsInstructions(string& option, string& str1,string& str2,string& str3);
void processOneWordInstructions(string& option, string& str1);
void processLabels(int index);
void move(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,std::vector<dbVariable>::iterator firstIt,std::vector<dwVariable>::iterator firstIt2,bool& isVariableFound1,bool& isVariableFound2,bool& isFirstVariableFound1,bool& isFirstVariableFound2,string& str1,string& str2,string& str3,int8_t *pmhl,int8_t *pnhl);
void add(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,int8_t *pmhl,int8_t *pnhl);
void sub(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,int8_t *pmhl,int8_t *pnhl);
unsigned short hexToDec(std::string hexString);
string decToHex(int n);
void toUpper(string& firstLine);
void toLower(string& firstLine);
void parseInput(string& line,ifstream& inFile);
inline std::string trim(std::string& str);
int determineValueOfInstruction(string &reg);
void determineReg(unsigned short **pmx, int8_t **pmhl, string& reg,bool& isVariableFound1,bool& isVariableFound2,std::vector<dbVariable>::iterator &it,std::vector<dwVariable>::iterator &it2);
int getIndexOfLabel(string & labelName);
void cmp(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,int8_t *pmhl,int8_t *pnhl);
void comparison(unsigned short firstValue,unsigned short secondValue);
int getOtherValue(string &str1);
int getVariableValue(string &str1);
unsigned short getVariableValue(bool& isVariableFound1,bool& isVariableFound2,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,string& str3);
bool checkBrackets(string& line);
inline string getLinestreamLine(istringstream& linestream,string& word,char option);
void determineLabelVariables();
void createLinesWithoutLabels(ifstream& inFile,string& stringName);
template <class regOne, class regTwo>  void moveValueToVariable(regOne& firstIt,regTwo *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3);
template <class regOne, class regTwo>  void moveValueToReg(regOne** firstReg, regTwo* secondReg,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3);
void constructMemory();
void setVariableAddress(string& variable,int address);
int getVariableAddress(string& variable);
int getVariableValueForMemoryAddress(int&  address);
void setVariableValue(string variableName,int value);
string cleanVariable(string variable);
string getVariableNameFromVariableAddress(string address);
bool isDigitDecimal(string variable,int digit);
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
      
      if(firstWord == "mov" || firstWord == "add" || firstWord == "sub"){
            
         if(line.find("offset") != string::npos) thirdWordsComma(linestream,secondWord,thirdWord,forthWord);
         else twoWordsComma(linestream,secondWord,thirdWord);
         processTwoWordsInstructions(firstWord,secondWord,thirdWord,forthWord);
      }else if(firstWord == "int" || firstWord == "mul" || firstWord == "div" || firstWord == "push" || firstWord == "pop"){
         getLinestreamLine(linestream,secondWord,' ');
         processOneWordInstructions(firstWord,secondWord);
      }else if(firstWord == "xor" || firstWord == "or" || firstWord == "and" || firstWord == "not"){
         twoWordsComma(linestream,secondWord,thirdWord);
         processTwoWordsInstructions(firstWord,secondWord,thirdWord,forthWord);
      }else if(firstWord == "rcl"){
         
      }else if(firstWord == "rcr"){
         
      }else if(firstWord == "shl"){
         
      }else if(firstWord == "shr"){
         
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
void twoWordsComma(istringstream& linestream,string& secondWord, string& thirdWord){
   getline(linestream,secondWord,',');
   trim(secondWord);
   getline(linestream,thirdWord);
   trim(thirdWord);
}

void thirdWordsComma(istringstream& linestream,string& secondWord, string& thirdWord, string& forthWord){
   getLinestreamLine(linestream,secondWord,',');
   getLinestreamLine(linestream,forthWord,' ');
   getLinestreamLine(linestream,thirdWord,' ');
}


void processOneWordInstructions(string& option, string& str1){
   if(option == "int" && str1 == "21h"){
      if(*pah == 1){
         char temp;
         cin >> temp;
         *pal = (int8_t)temp;
         cout << (char)(*pal);
      }else if(decToHex(*pah) == "2"){
         cout << (char)(*pdl) ;
      }else if(decToHex(*pah) == "8"){
         char temp;
         cin >> temp;
         *pal = (int8_t)temp;
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
   }else if(option == "div"){
      *pax /= (unsigned short)determineValueOfInstruction(str1);
   }else if(option == "mul"){
      *pax *= (unsigned short)determineValueOfInstruction(str1);
   }else if(option == "push"){
      unsigned short deger = (unsigned short)determineValueOfInstruction(str1);
      sp = sp << 8;
      sp = sp | deger;
      print_16bitregs() ; 

   }else if(option == "pop"){
      unsigned short *pmx = nullptr; 
      int8_t *pmhl = nullptr;
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


// PROCESS INSTRUCTIONS
void processTwoWordsInstructions(string& option, string& str1,string& str2,string& str3){
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      std::vector<dbVariable>::iterator firstIt;
      std::vector<dwVariable>::iterator firstIt2;
      int8_t *pmhl = nullptr;
      int8_t *pnhl = nullptr;
      unsigned short *pmx = nullptr; 
      unsigned short *pnx = nullptr; 

      bool isVariableFound1 = false;
      bool isVariableFound2 = false;
      bool isFirstVariableFound1 = false;
      bool isFirstVariableFound2 = false;

   // For Register Names

      determineReg(&pmx,&pmhl,str1,isFirstVariableFound1,isFirstVariableFound2,firstIt,firstIt2);
      if(str3 != "offset")
      determineReg(&pnx,&pnhl,str2,isVariableFound1,isVariableFound2,it,it2);
      

      if(option == "mov"){
         move(pmx,pnx,it,it2,firstIt,firstIt2,isVariableFound1,isVariableFound2,isFirstVariableFound1,isFirstVariableFound2,str1,str2,str3,pmhl,pnhl);
      }else if(option == "add"){
         add(pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3,pmhl,pnhl);
      }else if(option == "sub"){
         sub(pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3,pmhl,pnhl);
      }else if(option == "cmp"){
         cmp(pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3,pmhl,pnhl);
      }else if(option == "or"){
         int8_t eightBit = 0;
         unsigned short first = determineValueOfInstruction(str1);
         unsigned short second = determineValueOfInstruction(str2);
         unsigned short result = first | second;
         pnx = &result;
         eightBit = (int8_t)result;
         pnhl = &eightBit;
         move(pmx,pnx,it,it2,firstIt,firstIt2,isVariableFound1,isVariableFound2,isFirstVariableFound1,isFirstVariableFound2,str1,str2,str3,pmhl,pnhl);

      }else if(option == "and"){
         int8_t eightBit = 0;
         unsigned short first = determineValueOfInstruction(str1);
         unsigned short second = determineValueOfInstruction(str2);
         unsigned short result = first & second;
         pnx = &result;
         eightBit = (int8_t)result;
         pnhl = &eightBit;
         move(pmx,pnx,it,it2,firstIt,firstIt2,isVariableFound1,isVariableFound2,isFirstVariableFound1,isFirstVariableFound2,str1,str2,str3,pmhl,pnhl);

      }else if(option == "xor"){
         int8_t eightBit = 0;
         unsigned short first = determineValueOfInstruction(str1);
         unsigned short second = determineValueOfInstruction(str2);
         unsigned short result = first ^ second;
         pnx = &result;
         eightBit = (int8_t)result;
         pnhl = &eightBit;
         move(pmx,pnx,it,it2,firstIt,firstIt2,isVariableFound1,isVariableFound2,isFirstVariableFound1,isFirstVariableFound2,str1,str2,str3,pmhl,pnhl);

      }else if(option == "not"){
         int8_t eightBit = 0;
         unsigned short first = determineValueOfInstruction(str1);
         unsigned short result = ~first;
         pnx = &result;
         eightBit = (int8_t)result;
         pnhl = &eightBit;
         move(pmx,pnx,it,it2,firstIt,firstIt2,isVariableFound1,isVariableFound2,isFirstVariableFound1,isFirstVariableFound2,str1,str2,str3,pmhl,pnhl);

      }else if(option == "rcl"){
     
      }else if(option == "rcr"){
     
      }







   }

void orOfValues(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,std::vector<dbVariable>::iterator firstIt,std::vector<dwVariable>::iterator firstIt2,bool& isVariableFound1,bool& isVariableFound2,bool& isFirstVariableFound1,bool& isFirstVariableFound2,string& str2,string& str3,int8_t *pmhl,int8_t *pnhl) {

   char character;
   if(pmx != nullptr){
      if(pnx != nullptr){

      }else if(pnhl != nullptr){

      }else if(isVariableFound1){

      }else if(isVariableFound2){

      }else{

      }
   }else if(pmhl != nullptr){
      if(pnx != nullptr){

      }else if(pnhl != nullptr){

      }else if(isVariableFound1){

      }else if(isVariableFound2){

      }else{
         
      }
   }else if(isFirstVariableFound1){
      if(pnx != nullptr){

      }else if(pnhl != nullptr){

      }else if(isVariableFound1){

      }else if(isVariableFound2){

      }else{
         
      }
   }else if(isFirstVariableFound2){
      if(pnx != nullptr){

      }else if(pnhl != nullptr){

      }else if(isVariableFound1){

      }else if(isVariableFound2){

      }else{
         
      }
   }else{
      if(pnx != nullptr){

      }else if(pnhl != nullptr){

      }else if(isVariableFound1){

      }else if(isVariableFound2){

      }else{
         
      }
  
   }
   // moveValueToReg(&pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3);
   // else if(pmhl != nullptr){
   //    moveValueToReg(&pmhl,pnhl,it,it2,isVariableFound1,isVariableFound2,str2,str3);
   // }else if(isFirstVariableFound1){
   //    if(pnhl != nullptr) moveValueToVariable(firstIt,pnhl,it,it2,isVariableFound1,isVariableFound2,str2,str3);
   //    else moveValueToVariable(firstIt,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3);
   // }else if(isFirstVariableFound2){
   //    if(pnhl != nullptr) moveValueToVariable(firstIt2,pnhl,it,it2,isVariableFound1,isVariableFound2,str2,str3);
   //    else moveValueToVariable(firstIt2,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3);
   // }
}

void determineReg(unsigned short **pmx, int8_t **pmhl, string& reg,bool& isVariableFound1,bool& isVariableFound2,std::vector<dbVariable>::iterator &it,std::vector<dwVariable>::iterator &it2) {
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

string cleanVariable(string variable) {
   if(variable.find('[') != string::npos && variable.find(']') != string::npos){
      variable = variable.substr(variable.find_first_of('[')+1,variable.length());
      variable = variable.substr(0,variable.find_last_of(']'));
      if((variable.at(variable.length()-1) == 'h' || variable.at(variable.length()-1) == 'd') && variable.at(0) >= 48 && variable.at(0) <= 57) variable = variable.substr(0,variable.length()-1);
   } 
   return variable;
}

int determineValueOfInstruction(string &reg) {
   string resultReg= "";
   resultReg = cleanVariable(reg);
   int result = 0;
   std::vector<dbVariable>::iterator it;
   std::vector<dwVariable>::iterator it2;

   bool isVariableFound1 = false;
   bool isVariableFound2 = false;

   if(reg == "ax"){
      resultReg = *pax;
   }else if(reg == "bx"){
      resultReg = *pbx;
   }else if(reg == "cx"){
      resultReg = *pcx;
   }else if(reg == "dx"){
      resultReg = *pdx;
   }else if(reg == "ah"){
      resultReg = *pah;
   }else if(reg == "al"){
      resultReg = *pal;
   }else if(reg == "bh"){
      resultReg = *pbh;
   }else if(reg == "bl"){
      resultReg = *pbl;
   }else if(reg == "ch"){
      resultReg = *pch;
   }else if(reg == "cl"){
      resultReg = *pcl;
   }else if(reg == "dh"){
      resultReg = *pdh;
   }else if(reg == "dl"){
      resultReg = *pdl;
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
         result = (int8_t)((*it).value); 
      }else if(isVariableFound2){
            result = (unsigned short)((*it2).value); 
      }else{
         result = getOtherValue(reg);
      }
      
   }
   return result;
}


int getVariableValueForMemoryAddress(string&  address){
   return memory[stoi(cleanVariable(address))];
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

// WE SHOULD ADD CONDITION FOR OFFSET OF THE VARIABLE
unsigned short getVariableValue(bool& isVariableFound1,bool& isVariableFound2,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,string& str3) {
   short result = 0;
   if(isVariableFound1){
         result = (int8_t)((*it).value); 
   }else if(isVariableFound2){
         result = (unsigned short)((*it2).value); 
   }
   return result;
}


// If Decimal,HexaDecimal and Character But Not Reg,Not Variable
int getOtherValue(string &str1) {
   int result = 0;
   char character;

   if((48 <= str1.at(0) && str1.at(0) <= 57)){
      if(str1.at(str1.length()-1) == 'd'){
         str1 = str1.substr(0,str1.length()-1);
         result = stoi(str1);
      }else if((str1.at(str1.length()-1) == 'h')){
         str1 = str1.substr(0,str1.length()-1);
         result = hexToDec(str1);
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
void move(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,std::vector<dbVariable>::iterator firstIt,std::vector<dwVariable>::iterator firstIt2,bool& isVariableFound1,bool& isVariableFound2,bool& isFirstVariableFound1,bool& isFirstVariableFound2,string& str1,string& str2,string& str3,int8_t *pmhl,int8_t *pnhl) {
         char character;
         if(pmx != nullptr){
            moveValueToReg(&pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3);
         }else if(pmhl != nullptr){
            moveValueToReg(&pmhl,pnhl,it,it2,isVariableFound1,isVariableFound2,str2,str3);
         }else if(isFirstVariableFound1){
            if(pnhl != nullptr) moveValueToVariable(firstIt,pnhl,it,it2,isVariableFound1,isVariableFound2,str2,str3);
            else moveValueToVariable(firstIt,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3);
         }else if(isFirstVariableFound2){
            if(pnhl != nullptr) moveValueToVariable(firstIt2,pnhl,it,it2,isVariableFound1,isVariableFound2,str2,str3);
            else moveValueToVariable(firstIt2,pnx,it,it2,isVariableFound1,isVariableFound2,str2,str3);
         }else if(str1.at(0) == '[' && str1.at(str1.length()-1) == ']' && isDigitDecimal(str1,1)){
            
            if(str3 == "offset"){
               memory[stoi(cleanVariable(str1))] = getVariableAddress(str2);
               setVariableValue(getVariableNameFromVariableAddress(str1),getVariableAddress(str2));
               }
            else{
               memory[stoi(cleanVariable(str1))] = determineValueOfInstruction(str2);
               setVariableValue(getVariableNameFromVariableAddress(str1),determineValueOfInstruction(str2));
         }
      }  
}


bool isDigitDecimal(string variable,int digit){
   return 48 <= variable.at(digit) && variable.at(digit) <= 57;
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
            (*it2).value = value;
            isVariableFound2 = true;
            break;
         }
      }
      }
}

template <class regOne, class regTwo> 
void moveValueToReg(regOne** firstReg, regTwo* secondReg,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3){
   if(secondReg != nullptr){
      **firstReg = *secondReg;
   }
      // VARIABLE WILL BE MOVED.
   else if(isVariableFound1 || isVariableFound2){
      **firstReg = getVariableValue(isVariableFound1,isVariableFound2,it,it2,str3);
   }else if(str3 == "offset"){
      **firstReg = getVariableAddress(str2);
   }else{
      **firstReg = getOtherValue(str2);
   }

}

template <class regOne, class regTwo> 
void moveValueToVariable(regOne& firstIt,regTwo *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3){
   
      if(pnx != nullptr){
         (*firstIt).value = *pnx;
      }
         // VARIABLE WILL BE MOVED.
      else if(isVariableFound1 || isVariableFound2){
         (*firstIt).value = getVariableValue(isVariableFound1,isVariableFound2,it,it2,str3);
      }else if(str3 == "offset"){
         (*firstIt).value = getVariableAddress(str2);
      }
      else{
         (*firstIt).value= getOtherValue(str2);
      }

}

void add(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,int8_t *pmhl,int8_t *pnhl){
   char character;
   if(pmx != nullptr){
            if(pnx != nullptr){
               *pmx += *pnx;
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1 || isVariableFound2){
               *pmx += getVariableValue(isVariableFound1,isVariableFound2,it,it2,str3);
            }else{
              *pmx += getOtherValue(str2);
            }

            
         }else if(pmhl != nullptr){
            if(pnhl != nullptr){
               *pmhl += *pnhl;
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1 || isVariableFound2){
               *pmhl += getVariableValue(isVariableFound1,isVariableFound2,it,it2,str3);
            }else{
              *pmhl += getOtherValue(str2);
            }
         }
}

void sub(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,int8_t *pmhl,int8_t *pnhl){
   char character;
   if(pmx != nullptr){
            if(pnx != nullptr){
               *pmx -= *pnx;
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1 || isVariableFound2){
               *pmx -= getVariableValue(isVariableFound1,isVariableFound2,it,it2,str3);
            }else{
              *pmx -= getOtherValue(str2);
            }
         }else if(pmhl != nullptr){
            if(pnhl != nullptr){
               *pmhl -= *pnhl;
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1 || isVariableFound2){
               *pmhl -= getVariableValue(isVariableFound1,isVariableFound2,it,it2,str3);
            }else{
              *pmhl -= getOtherValue(str2);
            }
         }
}

void cmp(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,string& str3,int8_t *pmhl,int8_t *pnhl){
         char character;
         if(pmx != nullptr){
            if(pnx != nullptr){
               comparison(*pmx,*pnx);
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1 || isVariableFound2){
               comparison(*pmx,getVariableValue(isVariableFound1,isVariableFound2,it,it2,str3));
            }else{
               comparison(*pmx,getOtherValue(str2));
            }
         }else if(pmhl != nullptr){
            if(pnhl != nullptr){
               comparison(*pmhl,*pnhl);
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1 || isVariableFound2){
              comparison(*pmhl,getVariableValue(isVariableFound1,isVariableFound2,it,it2,str3));
            }else{
              comparison(*pmhl,getOtherValue(str2));
            }
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


// LABELS
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

void createLabel(ifstream& inFile,string& stringName){
   string labelName = "";
   istringstream linestream(stringName);
   getline(linestream,labelName,':');
   Label label;
   label.name = labelName;
   getLabelContent(label,inFile,stringName);
   
}

void createLinesWithoutLabels(ifstream& inFile,string& stringName){
   string labelName = "";
   Label label;
   label.name = "Without Labels";
   lineWithoutLabel = true;
   getLabelContent(label,inFile,stringName);
}

//  GETLINE - TRIM - toLOWERCASE

void getLineTrimLower(ifstream& inFile,string& firstLine){
   getline(inFile,firstLine);
   trim(firstLine);
   if(firstLine.find("\r") != string::npos) firstLine = firstLine.substr(0,firstLine.length()-1);
   toLower(firstLine);
   
}

// LOWER CASE
void toLower(string& firstLine){
   transform(firstLine.begin(), firstLine.end(), firstLine.begin(), ::tolower);
}
void toUpper(string& firstLine){
   transform(firstLine.begin(), firstLine.end(), firstLine.begin(), ::toupper);
}

// TRIM LINES
inline std::string trim(std::string& str) {
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    str.erase(str.find_last_not_of(' ')+1);         //surfixing spaces
    return str;
}

inline string getLinestreamLine(istringstream& linestream,string& word,char option) {
   do{
      getline(linestream,word,option);
      if(!linestream) break;
   }while(word == "");
      trim(word);
    return word;
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
void print_bits(datatype x)
{
    int i;

    for(i=8*sizeof(x)-1; i>=0; i--) {
        (x & (1 << i)) ? putchar('1') : putchar('0');
    }
    printf("\n");
}

template <class datatype> 
void print_hex(datatype x)
{
   if (sizeof(x) == 1) 
      printf("%02x\n",x); 
   else 
      printf("%04x\n",x); 
}

void print_16bitregs()
{
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

