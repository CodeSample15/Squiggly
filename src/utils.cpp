#include <memory>
#include <sstream>

#include "utils.hpp"
#include "built-in.hpp"
#include "runner.hpp"
#include "linter.hpp"
#include "external-libs.hpp"

using namespace Utils;

void skipBracketsWhenParsing(std::string& s, size_t& i, std::string& tmp); //when parsing a variable string name, skip everything inside of the brackets

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
                    if(s[i]=='[')
                        skipBracketsWhenParsing(s, i, temp);

                    temp += s[i];
                    if(i+1 >= s.length()) {
                        atEnd = true;
                        break; //no need to error, can simply be that the program reached the end of the argument string
                    }
                } while(isalpha(s[++i]) || s[i]==BUILT_IN_VAR_PREFIX || s[i]=='_' || s[i]=='.' || s[i]=='[');
                
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
                    if(input[i]=='[')
                        skipBracketsWhenParsing(input, i, temp);

                    temp += input[i];
                    if(i+1 >= input.length()) {
                        atEnd = true;
                        break; //no need to error, can simply be that the program reached the end of the argument string
                    }
                } while(isalpha(input[++i]) || input[i]==BUILT_IN_VAR_PREFIX || input[i]=='_' || input[i]=='.' || input[i]=='[');
                
                if(!atEnd) i--; //only do this if the scanner did not reach the end of the string (otherwise the scanner might go back and read the last character again and consider it a separate variable, causing an error)

                convertAndAppendVariable(ss, temp);
            } else {
                ss << input[i];
            }
        }

        if(expectedType != VarType::STRING) {
            //run expression through expression parser library
            double res = expressionToDouble(ss.str());

            if(res!=res) //Nan
                throwUtilError("Error parsing '" + input + "': Expression evaluated to NaN (Tip: make sure variable names are correct, values are valid for equation, and operations have correct format)");

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

/**
    Create a shared pointer of an array of squiggly variables
    Array is of size "size"
*/
std::shared_ptr<void> Utils::createEmptyShared(VarType type, int size) 
{
    std::vector<SVariable> arr;
    //init all values in array
    for(int i=0; i<size; i++) {
        SVariable temp;
        temp.name = std::to_string(i);
        temp.type = type;
        temp.ptr = createEmptyShared(type);
        temp.isArray = false;
        
        arr.push_back(temp);
    }

    return std::make_shared<std::vector<SVariable>>(arr);
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

/*
    When parsing a variable name, skip the '[]' in the string. This will allow array references to be parsed from variable strings
*/
void skipBracketsWhenParsing(std::string& s, size_t& i, std::string& tmp) 
{
    int nest_depth = 0; //keep track of how many nexted brackets are in this bracket statement 

    if(s[i] != '[')
        return; //error with the calling of this method, use this guard to prevent further errors

    for(; i<s.length(); i++) {
        if(s[i] == ']')
            nest_depth--;
        else if(s[i] == '[')
            nest_depth++;

        //exit loop once nest_depth is 0 (no more brackets)
        if(nest_depth==0)
            break;

        tmp += s[i]; //update the tmp variable that holds the string literal of the variable to be converted
    }
}

inline void throwUtilError(std::string message) {
    throw std::runtime_error("Squiggly util error: " + message);
}