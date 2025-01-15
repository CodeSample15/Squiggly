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
        bool isArray;
        int arrSize;
    } SVariable;

    std::string varTypeToString(VarType& type);
    VarType stringToVarType(std::string& str);

    std::string ParseString(std::string s); //convert string literal to string, regardless of value / concatenation
    SVariable convertToVariable(std::string input, VarType expectedType); //convert string to a SVariable (whether it's referencing another variable or just a plain string)

    std::shared_ptr<void> createEmptyShared(VarType type); //for initializing new variables with "empty" values
    std::shared_ptr<void> createEmptyShared(VarType type, int size); //for initializing new arrays with "empty" values
    std::shared_ptr<void> createSharedPtr(VarType type, double value); //create a new shared ptr with a specific type and cast value to that type

    //for singular values
    std::shared_ptr<void> createSharedPtr(std::string s);
    std::shared_ptr<void> createSharedPtr(int i);
    std::shared_ptr<void> createSharedPtr(double d);
    std::shared_ptr<void> createSharedPtr(float f);
    std::shared_ptr<void> createSharedPtr(bool b);

    //for arrays of values
    std::shared_ptr<void> createSharedPtr(std::string s, int arrSize);  //I know this method is going to make it more of a pain in the ass to make it so that multi-dimensional arrays can work in Squiggly, but quite frankly right now I don't care, just gotta get this dang project finished before it consumes me
    std::shared_ptr<void> createSharedPtr(int i, int arrSize);          //It would be more developer friendly to instead make an array of shared pointers that in turn point to values, but instead I'm making it so that each shared pointer points to it's own array of values, rather than an array of more shared pointers. My hope is that by doing so, I save some memory space. Will make a git commit as a checkpoint just in case
    std::shared_ptr<void> createSharedPtr(double d, int arrSize);
    std::shared_ptr<void> createSharedPtr(float f, int arrSize);
    std::shared_ptr<void> createSharedPtr(bool b, int arrSize);
}