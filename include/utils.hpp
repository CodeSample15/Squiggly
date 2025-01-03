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
        BOOL,
        OBJECT,
        TEXT
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

    std::shared_ptr<void> createEmptyShared(VarType type); //for initializing new variables with "empty" values
    std::shared_ptr<void> createSharedPtr(VarType type, double value); //create a new shared ptr with a specific type and cast value to that type
    std::shared_ptr<void> createSharedPtr(std::string s);
    std::shared_ptr<void> createSharedPtr(int i);
    std::shared_ptr<void> createSharedPtr(double d);
    std::shared_ptr<void> createSharedPtr(float f);
    std::shared_ptr<void> createSharedPtr(bool b);
}