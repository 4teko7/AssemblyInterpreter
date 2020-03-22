#include<iostream>
#include<stdlib.h>
#include<math.h>
 
 using namespace std;
unsigned short HexToDec(std::string hexString) {
  char *hexstr = &hexString.at(0);
  int length = 0;
  const int base = 16;     // Base of Hexadecimal Number
  unsigned short decnum = 0;
  int i;
//  cout << &(hexString.at(0)) << endl;
 cout << (*hexstr) << endl;
//  hexstr += 1;
//  hexstr += 1;
//  cout << *hexstr << endl;
  // Now Find the length of Hexadecimal Number
  for (; *hexstr != '\0'; hexstr++) {
      length++;
    }
 
  // Now Find Hexadecimal Number
  hexstr = &hexString.at(0);

  for (i = 0; *hexstr != '\0' && i < length; i++, hexstr++) {
      // Compare *hexstr with ASCII values
      if (*hexstr >= 48 && *hexstr <= 57) {  // is *hexstr Between 0-9
          decnum += (((int)(*hexstr)) - 48) * pow(base, length - i - 1);
        }
      else if ((*hexstr >= 65 && *hexstr <= 70))  {  // is *hexstr Between A-F
          decnum += (((int)(*hexstr)) - 55) * pow(base, length - i - 1);
        }
      else if (*hexstr >= 97 && *hexstr <= 102) {  // is *hexstr Between a-f
          decnum += (((int)(*hexstr)) - 87) * pow(base, length - i - 1);
        }
      else {
          std::cout<<"Invalid Hexadecimal Number \n";
 
        }
    }
  return decnum;
}
 
int main() {
  unsigned long decnum;
  char hex[9];
    std::string temp;
  std::cout<<" Enter 32-bit Hexadecimal Number : ";
  std::cin>>temp;
 
  decnum = HexToDec(temp);
  std::cout<<"Value in Decimal Number is "<<decnum<<"\n";
 
    return 0;
}
 