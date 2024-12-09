#include <sstream>

#include "utils.hpp"
#include "runner.hpp"

using namespace Utils;

void throwError(std::string message);

/*
    Convert string literal into raw string that Squiggly can work with (string appending with '+' and converting Squiggly variables
    into strings)
*/
std::string Utils::ParseString(std::string s) 
{
    std::stringstream ss;

    bool varScanMode = true; //when in varScan mode, Squiggly is looking for a variable name, when it's not in this mode, program will throw an error upon encountering 
    for(size_t i=0; i<s.length(); i++) {
        //loop through strings,
        if(s[i] == '"') {
            varScanMode = false; //expect strings from here on out or an append character (+)

            while(s[++i] != '"') {
                if(i>=s.length())
                    throwError("String left unclosed! Cannot parse string");

                ss << s[i]; //append characters to the string stream
            }
        } else {
            if(varScanMode) {
                //get a variable's value from the string
                std::string temp = "";
                do {
                    temp += s[i];
                    if(i+1 >= s.length())
                        break; //no need to error, can simply be that the program reached the end of the argument string
                } while(isalpha(s[++i]) || s[i]=='$');

                SVariable* var = Runner::fetchVariable(temp);
                if(var) {
                    switch(var->type) {
                        case VarType::STRING:
                            ss << *((std::string*)var->ptr.get()); //dear lord this is an ugly line of code. Casting the void pointer to a string pointer and then dereferncing 
                            break;
                        case VarType::INTEGER:
                            ss << *((int*)var->ptr.get());
                            break;
                        case VarType::DOUBLE:
                            ss << *((double*)var->ptr.get());
                            break;
                        case VarType::FLOAT:
                            ss << *((float*)var->ptr.get());
                            break;
                        case VarType::BOOL:
                            ss << (*((bool*)var->ptr.get()) ? "true" : "false");
                            break;
                    }
                }
                else {
                    throwError("Variable " + temp + " is not in scope!");
                }
            }
            else if(s[i] != '+') {
                //program is not in scan variable mode and there is no string to be found, program is structured incorrectly
                throwError("Invalid structuring of string.");
            }
            else {
                varScanMode = true; // + found, append new string or go back to varScanMode
            }
        }
    }

    return ss.str();
}

void throwError(std::string message) {
    throw std::runtime_error("Squiggly util error: " + message);
}