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
    unsigned short size = 0;
    int8_t value = 0;
    unsigned char character;
};


struct dwVariable{
    string name;
    unsigned short size = 0;
    unsigned short value = 0;
    unsigned char character;
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

// FUNCTIONS
void db(string& line);
void dw(string& line);
bool checkSpace(string& line);
bool checkQuotationMarks(string& line);
bool checkComma(string& line);
bool checkSemiColon(string& line);
bool checkint20h(string& line);
void getLabelContent(Label& label,ifstream& inFile);
void createLabel(ifstream& inFile,string& labelName);
void getLineTrimLower(ifstream& inFile,string& firstLine);
void printLabels();
void printVariables();
void twoWordsComma(istringstream& linestream,string& secondWord, string& thirdWord);
void processTwoWordsInstructions(string& option, string& str1,string& str2);
void processOneWordInstructions(string& option, string& str1);
void processLabels(int index);
void move(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,int8_t *pmhl,int8_t *pnhl);
void add(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,int8_t *pmhl,int8_t *pnhl);
void sub(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,int8_t *pmhl,int8_t *pnhl);
unsigned short hexToDec(std::string hexString);
string decToHex(int n);
void toUpper(string& firstLine);
void toLower(string& firstLine);
void parseInput(string& line,ifstream& inFile);
inline std::string trim(std::string& str);
int determineValueOfInstruction(string &reg);
void determineReg(unsigned short **pmx, int8_t **pmhl, string& reg);
int getIndexOfLabel(string & labelName);
void cmp(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,int8_t *pmhl,int8_t *pnhl);
void comparison(unsigned short firstValue,unsigned short secondValue);
int main()
{

    // Open the input and output files, check for failures
    ifstream inFile("atwon.txt");
    if (!inFile) { // operator! is synonymous to .fail(), checking if there was a failure
        cerr << "There was a problem opening \"" << "atwon.txt" << "\" as input file" << endl;
        return 1;
    }

   string firstLine;
   getLineTrimLower(inFile,firstLine);

   while(!checkint20h(firstLine) && !isint20h){
      parseInput(firstLine,inFile);  
      getLineTrimLower(inFile,firstLine);
   }

   printLabels();
   cout << " ********************************************** " << endl;
   // printVariables();
   processLabels(0);

   printLabels();

}

void processLabels(int index){
   int index2 = -1;
   string line = "";
   std::vector<string>::iterator it;
   bool newJmpFound = false;
   string newLabel = "";
   for (int i = index; i < labels.size() && !newJmpFound; i++)
   {
   for (it = labels.at(i).content.begin(); it != labels.at(i).content.end() && !newJmpFound; it++)
   {
      line = (*it);
      istringstream linestream(line);
      getline(linestream,firstWord,' ');
      if(firstWord.at(0) != 'j'){
         isJustAfterCompare = false;
         afterCompare.clear();
      }  
      if(firstWord == "mov" || firstWord == "add" || firstWord == "sub"){
         twoWordsComma(linestream,secondWord,thirdWord);
         processTwoWordsInstructions(firstWord,secondWord,thirdWord);
      }else if(firstWord == "int" || firstWord == "mul" || firstWord == "div" || firstWord == "push" || firstWord == "pop"){
         getline(linestream,secondWord);
         processOneWordInstructions(firstWord,secondWord);
      }else if(firstWord == "xor"){
         
      }else if(firstWord == "or"){
         
      }else if(firstWord == "and"){
         
      }else if(firstWord == "not"){
         
      }else if(firstWord == "rcl"){
         
      }else if(firstWord == "rcr"){
         
      }else if(firstWord == "shl"){
         
      }else if(firstWord == "shr"){
         
      }else if(firstWord == "nop"){
         
      }else if(firstWord == "cmp"){
         twoWordsComma(linestream,secondWord,thirdWord);
         processTwoWordsInstructions(firstWord,secondWord,thirdWord);
         isJustAfterCompare = true;
      }else if(firstWord.at(0) == 'j'){
         getline(linestream,secondWord);
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

            /* code */
         }
         }
         
      }
      
      // else if(firstWord == "jnz" || firstWord == "jne"){
         
      // }else if(firstWord == "je" || firstWord == "jz"){
         
      // }else if(firstWord == "ja" || firstWord == "jnbe"){
         
      // }else if(firstWord == "jbe"){
         
      // }else if(firstWord == "jnae" || firstWord == "jc" || firstWord == "jb"){
         
      // }else if(firstWord == "jnb" || firstWord == "jnc" || firstWord == "jae"){
         
      //  }


      //else if(firstWord.find(":") != string::npos){
      //    newLabelFound = true;
      //    newLabel = firstWord.substr(0,firstWord.length()-1);
      //    isJustAfterCompare = false;
      //    break;
      // }


      // getline(linestream,secondWord,',');
      // getline(linestream,thirdWord);

   }
      }
   // if(newLabelFound){
   //    int index2 = getIndexOfLabel(newLabel);
   //    if(index2 != -1) processLabels(index2);
   // }
if(index2 != -1) processLabels(index2);
}
void twoWordsComma(istringstream& linestream,string& secondWord, string& thirdWord){
   getline(linestream,secondWord,',');
   getline(linestream,thirdWord);
}


void processOneWordInstructions(string& option, string& str1){
   str1 = str1.substr(0,str1.length()-1);
   if(option == "int" && str1 == "21h"){
      if(*pah == 1){
         char temp;
         cin >> temp;
         *pal = (int8_t)temp;
         cout << *pal;
      }else if(decToHex(*pah) == "2"){
         cout << *pdl;
      }else if(decToHex(*pah) == "8"){
         char temp;
         cin >> temp;
         *pal = (int8_t)temp;
      }else if(decToHex(*pah) == "9"){
         if(*pdx == 255){
            cout << fromKeyboard;
         }else{
            cout << *pdx;
         }
      }else if(decToHex(*pah) == "A"){
         cin >> fromKeyboard;
         *pdx = 255;
      }
      cout.flush();
   }else if(option == "int" && str1 == "20h"){

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
      determineReg(&pmx,&pmhl,str1);
      unsigned short temp;
      temp = sp >> 8;
      temp = temp << 8;
      if(pmx != nullptr) *pmx = (sp - temp);
      else if(pmhl != nullptr) *pmhl = (sp - temp);
      sp = sp >> 8;
      print_16bitregs() ; 

      
   }

}


// PROCESS INSTRUCTIONS
void processTwoWordsInstructions(string& option, string& str1,string& str2){
      str2 = str2.substr(0,str2.length()-1);
      std::vector<dbVariable>::iterator it;
      std::vector<dwVariable>::iterator it2;
      int8_t *pmhl = nullptr;
      int8_t *pnhl = nullptr;
      unsigned short *pmx = nullptr; 
      unsigned short *pnx = nullptr; 

      bool isVariableFound1 = false;
      bool isVariableFound2 = false;

   // For Register Names

      determineReg(&pmx,&pmhl,str1);
      determineReg(&pnx,&pnhl,str2);

      for (it = dbVariables.begin(); it != dbVariables.end(); it++) {
         if(str2 == (*it).name){
            isVariableFound1 = true;
            break;
         }

      }
      if(!isVariableFound1){
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
      {
         if(str2 == (*it2).name){
            isVariableFound2 = true;
            break;
         }
      }
      }
      if(option == "mov"){
         move(pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,pmhl,pnhl);
      }else if(option == "add"){
         add(pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,pmhl,pnhl);
      }else if(option == "sub"){
         sub(pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,pmhl,pnhl);
      }else if(option == "cmp"){
         cmp(pmx,pnx,it,it2,isVariableFound1,isVariableFound2,str2,pmhl,pnhl);
      }






   }

void determineReg(unsigned short **pmx, int8_t **pmhl, string& reg) {

   if(reg == "ax"){
      *pmx = pax;
   }else if(reg == "bx"){
      *pmx = pbx;
   }else if(reg == "cx"){
      *pmx = pcx;
   }else if(reg == "dx"){
      *pmx = pdx;
   }else if(reg == "ah"){
      *pmhl = pah;
   }else if(reg == "al"){
      *pmhl = pal;
   }else if(reg == "bh"){
      *pmhl = pbh;
   }else if(reg == "bl"){
      *pmhl = pbl;
   }else if(reg == "ch"){
      *pmhl = pch;
   }else if(reg == "cl"){
      *pmhl = pcl;
   }else if(reg == "dh"){
      *pmhl = pdh;
   }else if(reg == "dl"){
      *pmhl = pdl;
   }

}



int determineValueOfInstruction(string &reg) {
   int result;
   std::vector<dbVariable>::iterator it;
   std::vector<dwVariable>::iterator it2;

   bool isVariableFound1 = false;
   bool isVariableFound2 = false;

   if(reg == "ax"){
      result = *pax;
   }else if(reg == "bx"){
      result = *pbx;
   }else if(reg == "cx"){
      result = *pcx;
   }else if(reg == "dx"){
      result = *pdx;
   }else if(reg == "ah"){
      result = *pah;
   }else if(reg == "al"){
      result = *pal;
   }else if(reg == "bh"){
      result = *pbh;
   }else if(reg == "bl"){
      result = *pbl;
   }else if(reg == "ch"){
      result = *pch;
   }else if(reg == "cl"){
      result = *pcl;
   }else if(reg == "dh"){
      result = *pdh;
   }else if(reg == "dl"){
      result = *pdl;
   }else{
      for (it = dbVariables.begin(); it != dbVariables.end(); it++)
      {
         if(reg == (*it).name){
            isVariableFound1 = true;
            break;
         }

      }
      if(!isVariableFound1){
      for (it2 = dwVariables.begin(); it2 != dwVariables.end(); it2++)
      {
         if(reg == (*it2).name){
            isVariableFound2 = true;
            break;
         }
      }
      }

    if(isVariableFound1){
         if((*it).character){
            result = (int8_t)((*it).character); 
         }else{
            result = (int8_t)((*it).value); 
         }
      }else if(isVariableFound2){
         if((*it2).character){
            result = (unsigned short)((*it2).character); 
         }else{
            result = (unsigned short)((*it2).value); 
         }
      }else if((48 <= reg.at(0) && reg.at(0) <= 57)){
               if(reg.at(reg.length()-1) == 'd'){
                  reg = reg.substr(0,reg.length()-1);
               }else if((reg.at(reg.length()-1) == 'h')){
                  reg = reg.substr(0,reg.length()-1);
               }
               result = (int8_t)hexToDec(reg);
      }else{
         char character;
         if(reg.at(0) == '\''){
            character = reg.at(1);
         }else if(reg.at(0) == '\"'){
            character = reg.at(1);
         }else{
            character = reg.at(0);
         }
         character = reg.at(0);
         result = (int8_t)character;
      }
      
   }
   return result;
}

//     M O V E    F U N C T I O N
void move(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,int8_t *pmhl,int8_t *pnhl){
         char character;
         if(pmx != nullptr){
            if(pnx != nullptr){
               *pmx = *pnx;
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1){
               if((*it).character){
                  *pmx= (int8_t)((*it).character); 
               }else{
                  *pmx = (int8_t)((*it).value); 
               }
            }else if(isVariableFound2){
               if((*it2).character){
                  *pmx = (unsigned short)((*it2).character); 
               }else{
                  *pmx = (unsigned short)((*it2).value); 
               }
            }
            
            else if(str2.at(str2.length()-1) == 'h'){
               str2 = str2.substr(0,str2.length()-1);
               *pmx = (unsigned short)stoi(str2);
            }else if((48 <= str2.at(0) && str2.at(0) <= 57)){
               if(str2.at(str2.length()-1) == 'd'){
                  str2 = str2.substr(0,str2.length()-1);
               }
               *pmx = (unsigned short)stoi(str2);
            }else{
               
               if(str2.at(0) == '\''){
                  character = str2.at(1);
               }else if(str2.at(0) == '\"'){
                  character = str2.at(1);
               }else{
                  character = str2.at(0);
               }
               *pmx = (int8_t)character;
            }
         }else if(pmhl != nullptr){
            if(pnhl != nullptr){
               *pmhl = *pnhl;
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1){
               if((*it).character){
                  *pmhl = (int8_t)((*it).character); 
               }else{
                  *pmhl = (int8_t)((*it).value); 
               }

            }else if(isVariableFound2){
               if((*it2).character){
                  *pmhl = (int8_t)((*it2).character); 
               }else{
                  *pmhl = (int8_t)((*it2).value); 
               }


            }else if((48 <= str2.at(0) && str2.at(0) <= 57)){
               if(str2.at(str2.length()-1) == 'd'){
                  str2 = str2.substr(0,str2.length()-1);
               }else if((str2.at(str2.length()-1) == 'h')){
                  str2 = str2.substr(0,str2.length()-1);
               }
               *pmhl = (int8_t)hexToDec(str2);
            }else{
               
               if(str2.at(0) == '\''){
                  character = str2.at(1);
               }else if(str2.at(0) == '\"'){
                  character = str2.at(1);
               }else{
                  character = str2.at(0);
               }
               character = str2.at(0);
               *pmhl = (int8_t)character;
            }
         }
}

void add(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,int8_t *pmhl,int8_t *pnhl){
   char character;
   if(pmx != nullptr){
            if(pnx != nullptr){
               *pmx += *pnx;
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1){
               if((*it).character){
                  *pmx += (unsigned short)((*it).character); 
               }else{
                  *pmx += (unsigned short)((*it).value); 
               }
            }else if(isVariableFound2){
               if((*it2).character){
                  *pmx += (unsigned short)((*it2).character); 
               }else{
                  *pmx += (unsigned short)((*it2).value); 
               }
            }
            
            else if(str2.at(str2.length()-1) == 'h'){
               str2 = str2.substr(0,str2.length()-1);
               *pmx += (unsigned short)stoi(str2);
            }else if((48 <= str2.at(0) && str2.at(0) <= 57)){
               if(str2.at(str2.length()-1) == 'd'){
                  str2 = str2.substr(0,str2.length()-1);
               }
               *pmx += (unsigned short)stoi(str2);
            }else{
               if(str2.at(0) == '\''){
                  character = str2.at(1);
               }else if(str2.at(0) == '\"'){
                  character = str2.at(1);
               }else{
                  character = str2.at(0);
               }
               *pmx += (unsigned short)character;
            }
         }else if(pmhl != nullptr){
            if(pnhl != nullptr){
               *pmhl += *pnhl;
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1){
               if((*it).character){
                  *pmhl += (int8_t)((*it).character); 
               }else{
                  *pmhl += (int8_t)((*it).value); 
               }

            }else if(isVariableFound2){
               if((*it2).character){
                  *pmhl += (int8_t)((*it2).character); 
               }else{
                  *pmhl += (int8_t)((*it2).value); 
               }


            }else if(str2.at(str2.length()-1) == 'h'){
               str2 = str2.substr(0,str2.length()-1);
               *pmhl += (int8_t)stoi(str2);
            }else if((48 <= str2.at(0) && str2.at(0) <= 57)){
               if(str2.at(str2.length()-1) == 'd'){
                  str2 = str2.substr(0,str2.length()-1);
               }
               *pmhl += (int8_t)stoi(str2);
            }else{
               if(str2.at(0) == '\''){
                  character = str2.at(1);
               }else if(str2.at(0) == '\"'){
                  character = str2.at(1);
               }else{
                  character = str2.at(0);
               }
               *pmhl += (int8_t)character;
            }
         }
}

void sub(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,int8_t *pmhl,int8_t *pnhl){
   char character;
   if(pmx != nullptr){
            if(pnx != nullptr){
               *pmx -= *pnx;
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1){
               if((*it).character){
                  *pmx -= (unsigned short)((*it).character); 
               }else{
                  *pmx -= (unsigned short)((*it).value); 
               }
            }else if(isVariableFound2){
               if((*it2).character){
                  *pmx -= (unsigned short)((*it2).character); 
               }else{
                  *pmx -= (unsigned short)((*it2).value); 
               }
            }
            
            else if(str2.at(str2.length()-1) == 'h'){
               str2 = str2.substr(0,str2.length()-1);
               *pmx -= (unsigned short)stoi(str2);
            }else if((48 <= str2.at(0) && str2.at(0) <= 57)){
               if(str2.at(str2.length()-1) == 'd'){
                  str2 = str2.substr(0,str2.length()-1);
               }
               *pmx -= (unsigned short)stoi(str2);
            }else{
               if(str2.at(0) == '\''){
                  character = str2.at(1);
               }else if(str2.at(0) == '\"'){
                  character = str2.at(1);
               }else{
                  character = str2.at(0);
               }
               *pmx -= (unsigned short)character;
            }
         }else if(pmhl != nullptr){
            if(pnhl != nullptr){
               *pmhl -= *pnhl;
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1){
               if((*it).character){
                  *pmhl -= (int8_t)((*it).character); 
               }else{
                  *pmhl -= (int8_t)((*it).value); 
               }

            }else if(isVariableFound2){
               if((*it2).character){
                  *pmhl -= (int8_t)((*it2).character); 
               }else{
                  *pmhl -= (int8_t)((*it2).value); 
               }


            }else if(str2.at(str2.length()-1) == 'h'){
               str2 = str2.substr(0,str2.length()-1);
               *pmhl -= (int8_t)stoi(str2);
            }else if((48 <= str2.at(0) && str2.at(0) <= 57)){
               if(str2.at(str2.length()-1) == 'd'){
                  str2 = str2.substr(0,str2.length()-1);
               }
               *pmhl -= (int8_t)stoi(str2);
            }else{
               if(str2.at(0) == '\''){
                  character = str2.at(1);
               }else if(str2.at(0) == '\"'){
                  character = str2.at(1);
               }else{
                  character = str2.at(0);
               }
               *pmhl -= (int8_t)character;
            }
         }
}

void cmp(unsigned short *pmx,unsigned short *pnx,std::vector<dbVariable>::iterator& it,std::vector<dwVariable>::iterator& it2,bool& isVariableFound1,bool& isVariableFound2,string& str2,int8_t *pmhl,int8_t *pnhl){
         char character;
         if(pmx != nullptr){
            if(pnx != nullptr){
               comparison(*pmx,*pnx);
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1){
               if((*it).character){
                  comparison(*pmx,(int8_t)((*it).character));
               }else{
                  comparison(*pmx,(int8_t)((*it).value));
               }
            }else if(isVariableFound2){
               if((*it2).character){
                  comparison(*pmx,(unsigned short)((*it2).character));
               }else{
                  comparison(*pmx,(unsigned short)((*it2).value));
               }
            }
            
            else if(str2.at(str2.length()-1) == 'h'){
               str2 = str2.substr(0,str2.length()-1);
               comparison(*pmx,(unsigned short)stoi(str2));

            }else if((48 <= str2.at(0) && str2.at(0) <= 57)){
               if(str2.at(str2.length()-1) == 'd'){
                  str2 = str2.substr(0,str2.length()-1);
               }
               comparison(*pmx,(unsigned short)stoi(str2));
               
            }else{
               
               if(str2.at(0) == '\''){
                  character = str2.at(1);
               }else if(str2.at(0) == '\"'){
                  character = str2.at(1);
               }else{
                  character = str2.at(0);
               }
               comparison(*pmx,(int8_t)character);

            }
         }else if(pmhl != nullptr){
            if(pnhl != nullptr){
               comparison(*pmhl,*pnhl);
            }
               // VARIABLE WILL BE MOVED.
            else if(isVariableFound1){
               if((*it).character){
                  comparison(*pmhl,(int8_t)((*it).character));
               }else{
                  comparison(*pmhl,(int8_t)((*it).value));
               }

            }else if(isVariableFound2){
               if((*it2).character){
                  comparison(*pmhl,(int8_t)((*it2).character));
               }else{
                  comparison(*pmhl,(int8_t)((*it2).value));
               }


            }else if((48 <= str2.at(0) && str2.at(0) <= 57)){
               if(str2.at(str2.length()-1) == 'd'){
                  str2 = str2.substr(0,str2.length()-1);
               }else if((str2.at(str2.length()-1) == 'h')){
                  str2 = str2.substr(0,str2.length()-1);
               }
               comparison(*pmhl,(int8_t)hexToDec(str2));
            }else{
               
               if(str2.at(0) == '\''){
                  character = str2.at(1);
               }else if(str2.at(0) == '\"'){
                  character = str2.at(1);
               }else{
                  character = str2.at(0);
               }
               character = str2.at(0);
               comparison(*pmhl,(int8_t)character);
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
//  MOVE REG TO REG
template <class regtype> 
void movRegToReg(regtype *preg1,regtype *preg2) 
{
     *preg1 = *preg2 ; 
}

//  INCREASE REG
template <class regtype> 
void incReg(regtype *preg1) 
{
     *preg1++ ; 
}

//  DECREASE REG
template <class regtype> 
void decReg(regtype *preg1) 
{
     *preg1-- ; 
}

//  MULTIPLY REG
template <class regtype> 
void mulReg(regtype *preg1) 
{
    *pax =  (*pax) * (*preg1);
}

//  DIVIDE REG
template <class regtype> 
void divReg(regtype *preg1) 
{
     *preg1++ ; 
}



// INPUT PARSING
void parseInput(string& line,ifstream& inFile){
   trim(line);
   bool isDb = line.find("db") != string::npos;
   bool isDw = line.find("dw") != string::npos;
   if(isDb || isDw){
      db(line);
      return;
   }
   if(checkSemiColon(line)){
      createLabel(inFile,line);
   }


   // if(checkSpace(line) || checkQuotationMarks(line) || checkComma(line)){

   // }


   istringstream linestream(line);

   bool space = line.find(' ') != string::npos;
   if(space){
      getline(linestream,firstWord,' ');
      getline(linestream,secondWord,',');
      getline(linestream,thirdWord);
      // cout << "FirstWord : " << firstWord << " - Second Word : " << secondWord << " - thirdWord : " << thirdWord << endl;
   }
   // bool comma = line.find(',') != string::npos;
   // bool semicolon = line.find(':') != string::npos;
}

// VARIABLES
void db(string& line){
   istringstream linestream(line);
   string temp,firstWord,secondWord,thirdWord,forthWord,fifthWord,sixthWord,seventhWord = "";
   if(checkSpace(line)){
      getline(linestream,firstWord,' ');
      getline(linestream,secondWord,' ');
      getline(linestream,thirdWord,' ');
      getline(linestream,forthWord,' ');
      getline(linestream,fifthWord,' ');
      // cout << "FirstWord : " << firstWord << " - Second Word : " << secondWord << " - thirdWord : " << thirdWord << " - ForthWord : " << forthWord << " - FIfth Word : " << fifthWord << endl;
      

      if(secondWord == "db"){
         dbVariable variable;
         variable.name = firstWord;
         if(checkQuotationMarks(thirdWord)){
            variable.character = thirdWord.at(0);
         }else{
            variable.value = stoi(thirdWord);
         }
         dbVariables.push_back(variable);
      }else{
         dwVariable variable;
         variable.name = firstWord;
         if(checkQuotationMarks(thirdWord)){
            variable.character = thirdWord.at(0);
         }else{
            variable.value = stoi(thirdWord);
         }
         dwVariables.push_back(variable);
      }
      
      
   }
}


// LABELS
void getLabelContent(Label& label,ifstream& inFile){
   string firstLine;
   getline(inFile,firstLine);
   trim(firstLine);
   toLower(firstLine);
   while(!checkint20h(firstLine) && !checkSemiColon(firstLine)){
      if(firstLine != "\r") label.content.push_back(firstLine);
      getLineTrimLower(inFile,firstLine);

   }
   labels.push_back(label);
   if(checkSemiColon(firstLine)){
      createLabel(inFile,firstLine);
   }
   if(checkint20h(firstLine)){
      isint20h = true;
   }
}

void createLabel(ifstream& inFile,string& stringName){
   string labelName = "";
   istringstream linestream(stringName);
   getline(linestream,labelName,':');
   Label label;
   label.name = labelName;
   
   getLabelContent(label,inFile);
   
}


//  GETLINE - TRIM - toLOWERCASE

void getLineTrimLower(ifstream& inFile,string& firstLine){
   getline(inFile,firstLine);
   trim(firstLine);
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
bool checkComma(string& line){
   return line.find(',') != string::npos;
}
bool checkSemiColon(string& line){
   return line.find(':') != string::npos;
}
bool checkint20h(string& line){
   return line.find("int 20h") != string::npos;
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


template <class regtype> 
void mov_reg_reg(regtype *preg1,regtype *preg2) 
{
     *preg1 = *preg2 ; 
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
   printf("SP:%04x\n",sp); 
}

