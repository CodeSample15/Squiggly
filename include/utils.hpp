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

    //convert string literal to string, regardless of value / concatenation
    std::string ParseString(std::string s);
}