#include<iostream>
#include<string.h>
using namespace std;
//convert hexadecimal to decimal
int convert(std::string hexString) {
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
int main() {
   char num[] = "250";
   cout<<num<<" after converting to deciaml becomes : "<<convert(num)<<endl;
   return 0;
}