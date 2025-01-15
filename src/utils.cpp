#include <memory>
#include <sstream>

#include "utils.hpp"
#include "built-in.hpp"
#include "runner.hpp"
#include "linter.hpp"
#include "external-libs.hpp"

using namespace Utils;
 
void convertAndAppendVariable(std::stringstream& ss, std::string& varName);
inline void throwUtilError(std::string message);

/*
    Allow functions to get the string version of a VarType
*/
std::string Utils::varTypeToString(VarType& type) {
    switch(type) {
        case VarType::NONE:
            return "NONE";
        case VarType::STRING:
            return "string";
        case VarType::INTEGER:
            return "int";
        case VarType::DOUBLE:
            return "double";
        case VarType::FLOAT:
            return "float";
        case VarType::BOOL:
            return "bool";
        case VarType::OBJECT:
            return "OBJECT";
        case VarType::TEXT:
            return "TEXT";
    }

    return "invalid";
}

VarType Utils::stringToVarType(std::string& str) {
    if(str=="string")
        return VarType::STRING;
    else if(str=="int")
        return VarType::INTEGER;
    else if(str=="double")
        return VarType::DOUBLE;
    else if(str=="float")
        return VarType::FLOAT;
    else if(str=="bool")
        return VarType::BOOL;
    else if(str=="OBJECT")
        return VarType::OBJECT;
    else if(str=="TEXT")
        return VarType::TEXT;

    return VarType::NONE;
}

/*
    Convert string literal into raw string that Squiggly can work with (string appending with '+' and converting Squiggly variables
    into strings)
*/
std::string Utils::ParseString(std::string s) 
{
    std::stringstream ss;

    bool varScanMode = true; //when in varScan mode, Squiggly is looking for a variable name, when it's not in this mode, program will throw an error upon encountering
    char stringNotation = '\0'; //what type of string is the user using (' or ")
    for(size_t i=0; i<s.length(); i++) {
        //loop through strings
        if(s[i] == '"' || s[i] == '\'') {
            varScanMode = false; //expect strings from here on out or an append character (+)
            stringNotation = s[i];

            while(s[++i] != stringNotation) {
                ss << s[i]; //append characters to the string stream

                if(i+1>=s.length())
                    throwUtilError("String left unclosed! Cannot parse string");
            }
        } else {
            if(varScanMode) {
                if(s[i]==STRING_CONCAT_CHAR)
                    continue;

                //get a variable's value from the string
                bool atEnd = false; //did the scanner reach the end of the string?
                std::string temp = "";
                do {
                    temp += s[i];
                    if(i+1 >= s.length()) {
                        atEnd = true;
                        break; //no need to error, can simply be that the program reached the end of the argument string
                    }
                } while(isalpha(s[++i]) || s[i]==BUILT_IN_VAR_PREFIX || s[i]=='_' || s[i]=='.');
                
                if(!atEnd) i--;

                convertAndAppendVariable(ss, temp);
            }
            else if(s[i] != STRING_CONCAT_CHAR) {
                //program is not in scan variable mode and there is no string to be found, program is structured incorrectly
                throwUtilError("Invalid structuring of string.");
            }
            else {
                varScanMode = true; // + found, append new string or go back to varScanMode
            }
        }
    }

    return ss.str();
}


SVariable Utils::convertToVariable(std::string input, VarType expectedType) {
    SVariable tmp;
    tmp.name = "tmp"; //main code has to set this manually
    tmp.type = VarType::NONE;

    if(expectedType == VarType::OBJECT || expectedType == VarType::TEXT) {
        SVariable* var = Runner::fetchVariable(input);
        if(var) {
            tmp.type = var->type;
            tmp.ptr = var->ptr;
        }
        else
            throwUtilError("Unable to fetch OBJECT variable \'" + input + "\'");
    }
    else if(input.find("\"") != std::string::npos || input.find("'") != std::string::npos) {
        //variable is a string literal probably
        std::string s = ParseString(input);

        tmp.type = VarType::STRING;
        tmp.ptr = std::make_shared<std::string>(s);
    } else {
        //replace variables in string with their values
        std::stringstream ss;
        for(size_t i=0; i<input.length(); i++) {
            if(isalpha(input[i]) || input[i]==BUILT_IN_VAR_PREFIX) {
                bool atEnd = false; //did the scanner reach the end of the string?
                std::string temp = "";
                do {
                    temp += input[i];
                    if(i+1 >= input.length()) {
                        atEnd = true;
                        break; //no need to error, can simply be that the program reached the end of the argument string
                    }
                } while(isalpha(input[++i]) || input[i]==BUILT_IN_VAR_PREFIX || input[i]=='_' || input[i]=='.');
                
                if(!atEnd) i--; //only do this if the scanner did not reach the end of the string (otherwise the scanner might go back and read the last character again and consider it a separate variable, causing an error)

                convertAndAppendVariable(ss, temp);
            } else {
                ss << input[i];
            }
        }

        if(expectedType != VarType::STRING) {
            //run expression through expression parser library
            double res = expressionToDouble(ss.str());
            tmp.type = expectedType;
            tmp.ptr = createSharedPtr(expectedType, res);
        } else {
            tmp.type = expectedType;
            tmp.ptr = createSharedPtr((std::string)ss.str());
        }
    }

    if(expectedType!=VarType::NONE && tmp.type!=expectedType)
        throwUtilError("Unable to convert <" + input + "> to '" + varTypeToString(expectedType) + "'");

    return tmp;
}

//default values for every Squiggly data type
std::shared_ptr<void> Utils::createEmptyShared(VarType type) {
    switch(type) {
        case VarType::STRING: return createSharedPtr((std::string)"");
        case VarType::INTEGER: return createSharedPtr((int)0);
        case VarType::DOUBLE: return createSharedPtr((double)0);
        case VarType::FLOAT: return createSharedPtr((float)0);
        case VarType::BOOL: return createSharedPtr(false);
        case VarType::OBJECT: return std::make_shared<BuiltIn::Object>(BuiltIn::Object());
        default: return createSharedPtr((int)0);
    }
}

std::shared_ptr<void> Utils::createEmptyShared(VarType type, int size) 
{
    switch(type) {
        case VarType::STRING: return std::shared_ptr<std::string[]>(new std::string[size]);
        case VarType::INTEGER: return std::shared_ptr<int[]>(new int[size]);
        case VarType::DOUBLE: return std::shared_ptr<double[]>(new double[size]);
        case VarType::FLOAT: return std::shared_ptr<float[]>(new float[size]);
        case VarType::BOOL: return std::shared_ptr<bool[]>(new bool[size]);
        case VarType::OBJECT: return std::shared_ptr<BuiltIn::Object[]>(new BuiltIn::Object[size]);
        default: return std::shared_ptr<int[]>(new int[size]);
    }
}

std::shared_ptr<void> Utils::createSharedPtr(VarType type, double value) {
    switch(type) {
        case VarType::STRING: return createSharedPtr(std::to_string(value));
        case VarType::INTEGER: return createSharedPtr((int)value);
        case VarType::DOUBLE: return createSharedPtr((double)value);
        case VarType::FLOAT: return createSharedPtr((float)value);
        case VarType::BOOL: return createSharedPtr(value!=0 ? true : false);
        default: return createSharedPtr((int)0);
    }
}

std::shared_ptr<void> Utils::createSharedPtr(std::string s) { return std::make_shared<std::string>(s); }
std::shared_ptr<void> Utils::createSharedPtr(int i) { return std::make_shared<int>(i); }
std::shared_ptr<void> Utils::createSharedPtr(double d) { return std::make_shared<double>(d); }
std::shared_ptr<void> Utils::createSharedPtr(float f) { return std::make_shared<float>(f); }
std::shared_ptr<void> Utils::createSharedPtr(bool b) { return std::make_shared<bool>(b); }

void convertAndAppendVariable(std::stringstream& ss, std::string& varName) {
    SVariable* var = Runner::fetchVariable(varName);
    
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
                ss << (*((bool*)var->ptr.get()) ? "1" : "0");
                break;
            default:
                ss << "1"; //safeguard against use of wrong datatypes, just convert to one
                break;
        }
    }
    else {
        throwUtilError("Variable '" + varName + "' is not in scope!");
    }
}

inline void throwUtilError(std::string message) {
    throw std::runtime_error("Squiggly util error: " + message);
}