#pragma once

#include <string>
#include <vector>

/*
    All of the built-in functions and objects that come with Squiggly
*/

namespace BuiltIn 
{
    //scan for a function and run it with the provided arguments
    void runFunction(std::string name, std::vector<std::string>& args);

    void Print(std::string message, bool newline=true);
}