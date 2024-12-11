#pragma once

#include <string>
#include <memory>

namespace Utils 
{
    enum VarType {
        NONE,
        STRING,
        INTEGER,
        DOUBLE,
        FLOAT,
        BOOL
    };

    typedef struct {
        std::string name;
        VarType type;
        std::shared_ptr<void> ptr;
    } SVariable;

    std::string varTypeToString(VarType& type);
    VarType stringToVarType(std::string& str);

    std::string ParseString(std::string s); //convert string literal to string, regardless of value / concatenation
    SVariable convertToVariable(std::string input, VarType expectedType); //convert string to a SVariable (whether it's referencing another variable or just a plain string)
}