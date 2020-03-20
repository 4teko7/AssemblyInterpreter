#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include<sstream>
#include <algorithm>
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
unsigned short sp = (2<<15)-2 ;



bool zf = 0;              // zero flag
bool sf = 0;              // sign flag 
bool cf = 0;              // carry flag
bool af = 0;              // auxillary flag 
bool of = 0;              // overflow flag

// initialize pointers 
unsigned short *pax = &ax ; 
unsigned short *pbx = &bx ; 
unsigned short *pcx = &cx ; 
unsigned short *pdx = &dx ; 

// unsigned short *pdi = &di ; 
// unsigned short *psi = &si ; 
// unsigned short *pbp = &bp ; 
// unsigned short *psp = &sp ; 


// note that x86 uses little endian, that is, least significat byte is stored in lowest byte 
unsigned char *pah = (unsigned char *) ( ( (unsigned char *) &ax) + 1) ;
unsigned char *pal = (unsigned char *) &ax  ;
unsigned char *pbh = (unsigned char *) ( ( (unsigned char *) &bx) + 1) ;
unsigned char *pbl = (unsigned char *) &ax  ; 
unsigned char *pch = (unsigned char *) ( ( (unsigned char *) &cx) + 1) ;
unsigned char *pcl = (unsigned char *) &ax  ; 
unsigned char *pdh = (unsigned char *) ( ( (unsigned char *) &dx) + 1) ;
unsigned char *pdl = (unsigned char *) &ax  ; 



using namespace std;
void toLower(string& firstLine);
void parseInput(string& line,ifstream& inFile);
inline std::string trim(std::string& str);

void db(string& line);
void dw(string& line);
bool checkSpace(string& line);
bool checkQuotationMarks(string& line);
bool checkComma(string& line);
bool checkSemiColon(string& line);
bool checkint20h(string& line);
bool isint20h = false;
struct dbVariable{
    string name;
    unsigned short size;
    unsigned short value = 0;
    string characters;
    //long long int step = 1;
};


struct dwVariable{
    string name;
    unsigned short size;
    unsigned short value = 0;
    string characters;
    //long long int step = 1;
};

struct Label{
    string name;
    vector<string> content;
};

vector<dbVariable> dbVariables;
// vector<vector<dbVariable>> vectorOfDbVariables;
vector<dwVariable> dwVariables;
// vector<vector<dwVariable>> vectorOfDwVariables;

vector<Label> labels;
void getLabelContent(Label& label,ifstream& inFile);
void createLabel(ifstream& inFile,string& labelName);
int main()
{







    // Open the input and output files, check for failures
    ifstream inFile("atwon.txt");
    if (!inFile) { // operator! is synonymous to .fail(), checking if there was a failure
        cerr << "There was a problem opening \"" << "atwon.txt" << "\" as input file" << endl;
        return 1;
    }









   string firstLine;
   getline(inFile,firstLine);
   toLower(firstLine);

   while(!checkint20h(firstLine) && !isint20h){
      parseInput(firstLine,inFile);  
      getline(inFile,firstLine);
      toLower(firstLine);
   }


   std::vector<string>::iterator it;
   
   for (int i = 0; i < labels.size(); i++)
   {
   for (it = labels.at(i).content.begin(); it != labels.at(i).content.end(); it++)
   {
      cout << "LABEL NAME : " << labels.at(i).name << " - LABEL CONTENT : " << (*it) << endl;

   }
      }

}


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
            variable.characters = thirdWord;
         }else{
            variable.value = stoi(thirdWord);
         }
         dbVariables.push_back(variable);
      }else{
         dwVariable variable;
         variable.name = firstWord;
         if(checkQuotationMarks(thirdWord)){
            variable.characters = thirdWord;
         }else{
            variable.value = stoi(thirdWord);
         }
         dwVariables.push_back(variable);
      }
      
      
   }
}

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

void getLabelContent(Label& label,ifstream& inFile){
   string firstLine;
   getline(inFile,firstLine);
   trim(firstLine);
   toLower(firstLine);
   while(!checkint20h(firstLine) && !checkSemiColon(firstLine)){
      if(firstLine != "\r") label.content.push_back(firstLine);
      getline(inFile,firstLine);
      
      trim(firstLine);
      toLower(firstLine);
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
   string temp,firstWord,secondWord,thirdWord,forthWord,fifthWord,sixthWord,seventhWord;

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
void toLower(string& firstLine){
   transform(firstLine.begin(), firstLine.end(), firstLine.begin(), ::tolower);
}

// 

inline std::string trim(std::string& str)
{
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    str.erase(str.find_last_not_of(' ')+1);         //surfixing spaces
    return str;
}









// std::stoi( str )   //String To Integer

// std::vector<dbVariable>::iterator it;
// for (it = dbVariables.begin(); it != dbVariables.end() ; it++)
// {
//    cout << (*it).name << " - value : " << (*it).value << " - Characters : " << (*it).characters << endl;
// }

// std::vector<dwVariable>::iterator it2;
// for (it2 = dwVariables.begin(); it2 != dwVariables.end() ; it2++)

// {
//    cout << (*it2).name << " - value : " << (*it2).value << " - Characters : " << (*it2).characters << endl;
// }











// template <class regtype> 
// void mov_reg_reg(regtype *preg1,regtype *preg2) 
// {
//      *preg1 = *preg2 ; 
// }

// template <class datatype> 
// void print_bits(datatype x)
// {
//     int i;

//     for(i=8*sizeof(x)-1; i>=0; i--) {
//         (x & (1 << i)) ? putchar('1') : putchar('0');
//     }
//     printf("\n");
// }

// template <class datatype> 
// void print_hex(datatype x)
// {
//    if (sizeof(x) == 1) 
//       printf("%02x\n",x); 
//    else 
//       printf("%04x\n",x); 
// }

// void print_16bitregs()
// {
//    printf("AX:%04x\n",ax); 
//    printf("BX:%04x\n",bx); 
//    printf("SP:%04x\n",sp); 
// }