#include <vector>

#include "runner.hpp"
#include "tokenizer.hpp"
#include "utils.hpp"

using namespace Runner;

//virtual memory buffers


//general function for running blocks of code
void runProgram(std::vector< Tokenizer::TokenizedLine >& tokens, size_t startIdx=0, size_t endIdx=0);

void Runner::executeVars()
{

}

void Runner::executeStart()
{

}

void Runner::executeUpdate()
{

}

void Runner::execute() 
{

}

/*
    Execute a block of the program.

    Starts from a particular location inside the given block of tokens, and ends at either the end of the token block if endIdx is 0, or at 
    position endIdx if the value is not 0
*/
void runProgram(std::vector< Tokenizer::TokenizedLine >& tokens, size_t startIdx, size_t endIdx) 
{
    
    endIdx = endIdx==0 ? tokens.size() : endIdx;
    
    //iterate through the program
    for(size_t prgCounter = startIdx; prgCounter < endIdx; prgCounter++) {

    }
}