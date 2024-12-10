#include <sstream>

#include "utils.hpp"
#include "runner.hpp"
#include "linter.hpp"
#include "built-in-funcs.hpp"


using namespace Utils;

inline void throwError(std::string message);

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
                            ss << *((std::string*)var->ptr.get()); //dear lord this is an ugly line of code. Casting the void pointer to a string pointer and then dereferencing 
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


SVariable Utils::convertToVariable(std::string input) {
    SVariable tmp;
    tmp.name = "tmp"; //main code has to set this manually

    if(input.find("\"") != std::string::npos || input.find("'") != std::string::npos) {
        //variable is a string literal probably
        std::string s = ParseString(input);

        tmp.type = VarType::STRING;
        tmp.ptr = std::make_shared<std::string>(s);
    } else if(input[0]==BUILT_IN_VAR_PREFIX) {
        //reference to Squiggly built in variable
        //TODO: fetch built in variable value

    } else {
        //regular reference to squiggly variable
        SVariable* fetched = Runner::fetchVariable(input);

        if(fetched) {
            tmp.type = fetched->type;
            tmp.ptr = fetched->ptr; //pass by reference
        } else {
            throwError("Unable to convert variable " + input + " to value");
        }
    }

    return tmp;
}

inline void throwError(std::string message) {
    throw std::runtime_error("Squiggly util error: " + message);
}