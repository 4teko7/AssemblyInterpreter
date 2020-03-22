// C++ program to convert a decimal 
// number to hexadecimal number 

#include<iostream> 
using namespace std; 

string decToHexa(int n) { 
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

int main() 
{ 
    int n = 2730; 
      
    cout << decToHexa(n) << endl; 
      
    return 0; 
} 