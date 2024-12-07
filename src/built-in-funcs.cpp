#include <stdexcept>
#include <iostream>

#include "built-in-funcs.hpp"

using namespace BuiltIn;

void throwError(std::string message);

void BuiltIn::runFunction(std::string name, std::vector<std::string>& args) 
{
    //ho boy, ignore the inefficiencies displayed in this code
    if(name.compare("PRINT")==0) {
        Print(args[0]);
    }
    else {
        //throw error
        throwError("Function " + name + " not found in built in function list");
    }
}

/*
    These functions will need to be changed here when porting to arduino:
*/

void BuiltIn::Print(std::string m, bool newline) {
    std::cout << m;
    if(newline)
        std::cout << std::endl;
    //Serial.print() //for arduino
}



void throwError(std::string message) {
    throw std::runtime_error("Built in function failed! : " + message);
}