#include <stdexcept>
#include <iostream>

#include "built-in-funcs.hpp"
#include "utils.hpp"

using namespace BuiltIn;

inline void throwError(std::string message);

void BuiltIn::runFunction(std::string name, std::vector<std::string>& args) 
{
    //ho boy, ignore the inefficiencies displayed in this code
    if(name.compare("PRINT")==0) {
        if(args.size() != 1)
            throwError("PRINT: expected 1 argument, got " + std::to_string(args.size()));

        std::string s = *((std::string*)Utils::convertToVariable(args[0], Utils::VarType::STRING).ptr.get());
        Print(s);
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

void BuiltIn::Print(char c, bool newline) {
    std::cout << c;
    if(newline)
        std::cout << std::endl;
    //Serial.print() //for arduino
}

void BuiltIn::PrintErr(std::string m) {
    std::cerr << m << std::endl;
}

void BuiltIn::PrintErr(char c) {
    std::cerr << c << std::endl;
}

inline void throwError(std::string message) {
    throw std::runtime_error("Built in function failed! : " + message);
}