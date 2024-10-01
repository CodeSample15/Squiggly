#include "tokenizer.h"
#include "linter.h"

using namespace Tokenizer;

//init extern variables
std::vector<TokenizedLine> varsBlock_tok = std::vector<TokenizedLine>();
std::vector<TokenizedLine> startBlock_tok = std::vector<TokenizedLine>();
std::vector<TokenizedLine> mainLoop_tok = std::vector<TokenizedLine>();
std::vector<std::vector<TokenizedLine>> functions_tok = std::vector<std::vector<TokenizedLine>>();


//helper functions
void clearTokenized();
void findCode(std::string header, std::vector<std::string>& lines, size_t& start, size_t& end);
void findOpenCloseBraces(std::vector<std::string>& lines, size_t& start, size_t& end);
void tokenizeSection(std::vector<std::string>& lines, std::vector<TokenizedLine>& tokenBuff, size_t start, size_t end);

/*
    Parse the code line by line and build a tokenized version of the script.
    Assumes that the linter did its job and the syntax of the code is correct.
    Uses the provided vector of lines to fill the tokenized vectors with TOkenizedLine structs
*/
void Tokenizer::tokenize(std::vector<std::string>& lines)
{
    //clear out any script that might still be in memory
    clearTokenized();

    int start = 0;
    int end = 0;

    findCode(VAR_FUNC_HEAD, lines, start, end);
    tokenizeSection(lines, varsBlock_tok, start, end);

    //clear memory after tokenizing
    lines.clear();
}

//reset all of the tokenized objects to be empty (clear memory of old program if it exists)
void clearTokenized() {
    varsBlock_tok.clear();
    startBlock_tok.clear();
    mainLoop_tok.clear();

    //clear all of the user defined functions stored in the array
    for(size_t i=0; i<functions_tok.size(); i++) {
        functions_tok[i].clear();
    }
    functions_tok.clear();
}

//finds the start and end locations for code segments with specific headers (system functions called automatically)
void findCode(std::string header, std::vector<std::string>& lines, size_t& start, size_t& end) 
{
    //scan for vars function (declares all variables)
    size_t found;
    for(size_t i=0; i<lines.size(); i++) {
        found = lines[i].find(header);
        if(found != std::string::npos) {
            start = i;
            findOpenCloseBraces(start, end);
        }
    }
}

//from any start position, find the location of where the opening braces are closed
void findOpenCloseBraces(std::vector<std::string>& lines, size_t& start, size_t& end) 
{
    std::vector<char> stack;

    //find first opening brace

    for(size_t i=start; i<lines.size(); i++) {
        //for(int i=0; i<)
    }
}

void tokenizeSection(std::vector<std::string>& lines, std::vector<TokenizedLine>& tokenBuff, size_t start, size_t end) {
    
}