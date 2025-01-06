#pragma once

#include <string>
#include <vector>

#include "utils.hpp"

/*
    All of the built-in functions and objects that come with Squiggly
*/

namespace BuiltIn 
{
    //scan for a function and run it with the provided arguments
    void runFunction(std::string name, std::vector<std::string>& args);

    void Print(std::string message, bool newline=true);
    void Print(char c, bool newline=true);

    void PrintErr(std::string message); //same thing as print, just print to error stream
    void PrintErr(char c);

    enum ObjectShape {
        RECT,
        TRIANGLE,
        CIRCLE,
        CROSS
    };

    //built in class types
    class Object {
        public:
            Object();

            void callFunction(std::string name, std::vector<Utils::SVariable>& args);
            Utils::SVariable* fetchVariable(std::string name);

        private:
            //all of these variables / functions will be accessible from from Squiggly scripts
            Utils::SVariable x;
            Utils::SVariable y;
            Utils::SVariable width;
            Utils::SVariable height;
            Utils::SVariable rotation;

            ObjectShape shape;
            uint8_t color[3];

            void draw();
            void isTouching(Object& other);
    };

    class Text { 
        //TODO: this still needs to be implemented lol
    };
}