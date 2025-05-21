#include <stdexcept>
#include <iostream>
#include <climits>
#include <cstdlib>
#include <algorithm>

#include "built-in.hpp"
#include "utils.hpp"
#include "runner.hpp"
#include "linter.hpp"
#include "graphics.hpp"

using namespace BuiltIn;

//for returning values to built in variables
int* INT_RET_PTR = nullptr;
float* FLOAT_RET_PTR = nullptr;

inline void throwError(std::string message);

void BuiltIn::fetchBuiltInReturnVariables() {
    //fetch int return variable
    std::string flagName = INT_RETURN_BUCKET_VAR_NAME;
    flagName.insert(0, 1, BUILT_IN_VAR_PREFIX);
    INT_RET_PTR = (int*)Runner::fetchVariable(flagName)->ptr.get();

    //fetch float return variable
    flagName = FLOAT_RETURN_BUCKET_VAR_NAME;
    flagName.insert(0, 1, BUILT_IN_VAR_PREFIX);
    FLOAT_RET_PTR = (float*)Runner::fetchVariable(flagName)->ptr.get();

    //treat this as an initialization call and seed rand
    srand(time(0));
}

/**
 * @brief Run a built in function and pass arguments as strings
 * 
 * @param name 
 * @param args 
 */
void BuiltIn::runFunction(std::string name, std::vector<std::string>& args) 
{
    if(name == "PRINT") {
        if(args.size() != 1)
            throwError("PRINT: expected 1 argument, got " + std::to_string(args.size()));

        std::string s = *((std::string*)Utils::convertToVariable(args[0], Utils::VarType::STRING).ptr.get());
        Print(s);
    }
    else if(name == "LEN") {
        if(args.size() != 1)
            throwError("LEN: expected 1 argument, got " + std::to_string(args.size()));

        //attempt to fetch array variable from runner
        Utils::SVariable* arrVar = Runner::fetchVariable(args[0], true);

        if(!arrVar || !arrVar->isArray)
            throwError("Value passed to LEN either doesn't exist or is not an array.");

        //assign return value
        *INT_RET_PTR = arrVar->arrSize;
    }
    else if(name == "I_RAND") {
        if(args.size() != 2)
            throwError("I_RAND: expected 2 arguments, got " + std::to_string(args.size()));

        //convert minimum and maximum arguments to int literals
        int min = *((int*)Utils::convertToVariable(args[0], Utils::VarType::INTEGER).ptr.get());
        int max = *((int*)Utils::convertToVariable(args[1], Utils::VarType::INTEGER).ptr.get());

        //generate random number and store in I_RET squiggly flag
        GenRandNum(min, max);
    }
    else if(name == "F_RAND") {
        if(args.size() != 0)
            throwError("F_RAND: no arguments expected, got " + std::to_string(args.size()));

        GenRandNum();
    }
    else if(name == "DRAW_LINE") {
        if(args.size() != 4 && args.size() != 7)
            throwError("DRAW_LINE: expected either 4 or 7 arguments, got " + std::to_string(args.size()));

        SGraphics::Color color;
        SGraphics::pixel p1;
        SGraphics::pixel p2;

        p1.x = *((float*)Utils::convertToVariable(args[0], Utils::VarType::FLOAT).ptr.get());
        p1.y = *((float*)Utils::convertToVariable(args[1], Utils::VarType::FLOAT).ptr.get());
        p2.x = *((float*)Utils::convertToVariable(args[2], Utils::VarType::FLOAT).ptr.get());
        p2.y = *((float*)Utils::convertToVariable(args[3], Utils::VarType::FLOAT).ptr.get());

        if(args.size() == 7) {
            //user did in fact specify color, use it
            int r = *((int*)Utils::convertToVariable(args[4], Utils::VarType::INTEGER).ptr.get());
            int g = *((int*)Utils::convertToVariable(args[5], Utils::VarType::INTEGER).ptr.get());
            int b = *((int*)Utils::convertToVariable(args[6], Utils::VarType::INTEGER).ptr.get());

            //clamp values to avoid over/underflow
            color.r = std::clamp(r, 0, 255);
            color.g = std::clamp(g, 0, 255);
            color.b = std::clamp(b, 0, 255);
        } else {
            color.r = 255;
            color.g = 255;
            color.b = 255;
        }

        SGraphics::draw_line(p1, p2, color);
    }
    else {
        //throw error
        throwError("Function " + name + " not found in built in function list");
    }
}

/*
    These functions will need to be changed here when porting to arduino:
*/

void BuiltIn::Print(std::string m, bool newline) {
    std::cout << m;
    if(newline)
        std::cout << std::endl;
}

void BuiltIn::Print(char c, bool newline) {
    std::cout << c;
    if(newline)
        std::cout << std::endl;
}

void BuiltIn::PrintErr(std::string m) {
    std::cerr << m << std::endl;
}

void BuiltIn::PrintErr(char c) {
    std::cerr << c << std::endl;
}

void BuiltIn::GenRandNum(int min, int max) {
    *INT_RET_PTR = (rand()%(max-min)) + min;
}

void BuiltIn::GenRandNum() {
    *FLOAT_RET_PTR = (double)rand()/RAND_MAX;
}

inline void throwError(std::string message) {
    throw std::runtime_error("Built in function failed! : " + message);
}