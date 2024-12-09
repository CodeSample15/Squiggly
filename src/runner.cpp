#include <vector>
#include <memory>
#include <stdexcept>

#include "runner.hpp"
#include "tokenizer.hpp"
#include "built-in-funcs.hpp"
#include "utils.hpp"

using namespace Runner;

//virtual memory buffers
std::vector<Utils::SVariable> gVars;    //global variables
std::vector<Utils::SVariable> sVars;    //stack variables

//useful functions
void runProgram(std::vector<TOKENIZED_PTR>& tokens, size_t startIdx=0, size_t endIdx=0); //general function for running blocks of code
void runUserFunction(std::string name, std::vector<std::string>& args); //find a user defined function and run
inline void throwError(std::string message); //throw a runner error

void Runner::executeVars() { runProgram(varsBlock_tok); }
void Runner::executeStart() { runProgram(startBlock_tok); }
void Runner::executeUpdate() { runProgram(mainLoop_tok); }

//main execute function with loop
void Runner::execute() 
{
    executeVars();
    executeStart();

    //TODO: put in escapable while loop
    executeUpdate();
}

Utils::SVariable* Runner::fetchVariable(std::string name) 
{
    //search local stack frame first (start at most recently pushed variable and work backwards, hopefully should add some efficiency)
    for(size_t i=sVars.size()-1; i>=0; i--) {
        if(sVars[i].name == name)
            return &sVars[i];
    }

    //search global variables
    for(Utils::SVariable& var : gVars) {
        if(var.name == name)
            return &var;
    }
}

/*
    Execute a block of the program.

    Starts from a particular location inside the given block of tokens, and ends at either the end of the token block if endIdx is 0, or at 
    position endIdx if the value is not 0
*/
void runProgram(std::vector<TOKENIZED_PTR>& tokens, size_t startIdx, size_t endIdx) 
{
    endIdx = endIdx==0 ? tokens.size() : endIdx;

    Tokenizer::CallLine* callLine;
    //Tokenizer::BranchLine* branchLine;
    //Tokenizer::LoopLine* loopLine;
    //Tokenizer::AssignLine* assignLine;
    //Tokenizer::DeclareLine* declareLine;
    //Tokenizer::FuncNameLine* funcNameLine;
    
    //iterate through the program
    for(size_t prgCounter = startIdx; prgCounter < endIdx; prgCounter++) {
        TOKENIZED_PTR line = tokens[prgCounter];

        switch(line->type) {
            case Tokenizer::LineType::CALL:
                callLine = (Tokenizer::CallLine*)line.get();
                runUserFunction(callLine->callFuncName, callLine->args);
                break;

            case Tokenizer::LineType::BI_CALL:
                callLine = (Tokenizer::CallLine*)line.get();
                BuiltIn::runFunction(callLine->callFuncName, callLine->args);
                break;

            default:
                BuiltIn::Print("Unknown line!");
                break;
        }
    }


}

/*
    Search the tokenized user functions for a function call name
*/
void runUserFunction(std::string name, std::vector<std::string>& args) {
    Tokenizer::FuncNameLine* tmp;
    for(std::vector<TOKENIZED_PTR>& function : functions_tok) {
        if(function[0]->type == Tokenizer::LineType::FUNC_NAME) {
            tmp = (Tokenizer::FuncNameLine*)function[0].get(); //cast into func name line pointer

            //compare name of function with name of function being called
            if(tmp->funcName == name) {
                //TODO: add arguments to stack

                runProgram(function, 1);
                break;
            }
        } else {
            throwError("Tokenizer screwed up function tokenizing!"); //idealy, the programmer should NEVER get this error, I'm just putting this here for Squiggly development screw ups
        }
    }
}

inline void throwError(std::string message) {
    throw std::runtime_error("Program Runner failed! : " + message);
}