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

    union value {
        size_t l;
        int i;
        float f;
        double d;
        char c[8];
    };

    typedef struct {
        VarType type;
        
    } SVariable;

    //convert string to string literal, regardless of value / concatenation
    std::string ParseString(std::string s);
}