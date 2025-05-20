#include <stdexcept>
#include <iostream>
#include <climits>
#include <cstdlib>

#include "built-in.hpp"
#include "utils.hpp"
#include "runner.hpp"
#include "linter.hpp"

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

        int min = *((int*)Utils::convertToVariable(args[0], Utils::VarType::INTEGER).ptr.get());
        int max = *((int*)Utils::convertToVariable(args[1], Utils::VarType::INTEGER).ptr.get());

        GenRandNum(min, max);
    }
    else if(name == "F_RAND") {
        if(args.size() != 0)
            throwError("F_RAND: no arguments expected, got " + std::to_string(args.size()));

        GenRandNum();
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
    //Serial.print() //for arduino
}

void BuiltIn::Print(char c, bool newline) {
    std::cout << c;
    if(newline)
        std::cout << std::endl;
    //Serial.print() //for arduino
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