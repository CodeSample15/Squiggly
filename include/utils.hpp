#pragma once

#include <string>
#include <memory>

namespace Utils 
{
    enum VarType {
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

    std::string ParseString(std::string s); //convert string literal to string, regardless of value / concatenation
    SVariable convertToVariable(std::string input); //convert strint to a SVariable (whether it's referencing another variable or just a plain string)
}