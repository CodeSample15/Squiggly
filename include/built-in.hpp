#pragma once

#include <cstdint>
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

    enum ObjectShape { //not implemented yet
        RECT,
        TRIANGLE,
        CIRCLE,
        CROSS
    };

    //built in class types
    class Object {
        public:
            Object();

            float getX();
            float getY();
            float getWidth();
            float getHeight();
            void getColor(uint8_t buffer[3]);

            void callFunction(std::string name, std::vector<std::string>& args);
            Utils::SVariable* fetchVariable(std::string name);

            void setColor(uint8_t r, uint8_t g, uint8_t b);

        private:
            //all of these variables / functions will be accessible from from Squiggly scripts
            Utils::SVariable x;
            Utils::SVariable y;
            Utils::SVariable width;
            Utils::SVariable height;
            Utils::SVariable rotation;

            Utils::SVariable color_r;
            Utils::SVariable color_g;
            Utils::SVariable color_b;

            ObjectShape shape;

            void draw();
            bool isTouching(Object& other);

            //pointers to variables in main memory that objects will need access to
            bool* collisionFlag;
    };

    class Text { 
        //TODO: this still needs to be implemented lol
    };
}