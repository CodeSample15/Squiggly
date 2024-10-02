#include "tokenizer.h"
#include "linter.h"

using namespace Tokenizer;

//init extern variables
std::vector<TokenizedLine> varsBlock_tok = std::vector<TokenizedLine>();
std::vector<TokenizedLine> startBlock_tok = std::vector<TokenizedLine>();
std::vector<TokenizedLine> mainLoop_tok = std::vector<TokenizedLine>();
std::vector<std::vector<TokenizedLine>> functions_tok = std::vector<std::vector<TokenizedLine>>();


//helper functions prototypes--------------------------------------------------------------------------------------------
void clearTokenized();

void findCode(std::string header, std::vector<std::string>& lines, size_t& start, size_t& end, bool err=false);

void findOpenCloseBraces(std::vector<std::string>& lines, size_t& start, size_t& end);

void tokenizeSection(std::vector<std::string>& lines, std::vector<TokenizedLine>& tokenBuff, size_t start, size_t end);


/*
    Parse the code line by line and build a tokenized version of the script.
    Assumes that the linter did its job and the syntax of the code is correct.
    Uses the provided vector of lines to fill the tokenized vectors with TOkenizedLine structs
*/
void Tokenizer::tokenize(std::vector<std::string>& lines)
{
    std::cout << "Tokenizing code...\t";

    //clear out any script that might still be in memory
    clearTokenized();

    size_t start = 0;
    size_t end = 0;

    //find and tokenize all of the non optional functions in the code
    findCode(VAR_FUNC_HEAD, lines, start, end, true);
    tokenizeSection(lines, varsBlock_tok, start, end);

    // findCode(START_FUNC_HEAD, lines, start, end, true);
    // tokenizeSection(lines, varsBlock_tok, start, end);

    // findCode(UPDATE_FUNC_HEAD, lines, start, end, true);
    // tokenizeSection(lines, varsBlock_tok, start, end);

    //clear memory after tokenizing
    lines.clear();

    std::cout << "Done" << std::endl;;
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
void findCode(std::string header, std::vector<std::string>& lines, size_t& start, size_t& end, bool err) 
{
    //scan for vars function (declares all variables)
    size_t found;
    for(size_t i=0; i<lines.size(); i++) {
        found = lines[i].find(header);
        if(found != std::string::npos) {
            start = i;
            findOpenCloseBraces(lines, start, end);
            return;
        }
    }

    if(err) { //show an error if the function was not in fact found (again, linter should take care of this, but just in case...)
        clearTokenized();
        lines.clear();
        std::cerr << "Failed to find required " << header << " function. Quitting..." << std::endl;
        std::exit(0);
    }
}

//from any start position, find the location of where the opening braces are closed
//once again, this method assumes that the linter has done its job correctly in the syntax
//check and there are NO MISSING BRACES
void findOpenCloseBraces(std::vector<std::string>& lines, size_t& start, size_t& end) 
{
    //each opening brace will be pushed to the stack. each closing brace will remove from the stack
    //once the stack is empty, the program will stop, having found the last brace to the function specified
    std::vector<char> stack;

    bool foundFirst = false; //don't start considering a stack empty until the first open brace is found

    //find first opening brace
    for(size_t i=start; i<lines.size(); i++) {

        //loop through each line to find an opening brace
        for(char letter : lines[i]) {
            if(letter == '{') {
                if(!foundFirst) {
                    start = i; //mark the new start location
                    foundFirst = true;
                }
                stack.push_back(letter);
            }
            else if(letter == '}' && foundFirst) {
                stack.erase(stack.begin() + stack.size()-1);
            }

            if(stack.size() == 0 && foundFirst) {
                end = i+1;
                return;
            }
        }
    }
}

/*
    Go to the section of the code specified with the start and end lines variables and go
    line by line to tokenize the program into a tokenBuff
*/
void tokenizeSection(std::vector<std::string>& lines, std::vector<TokenizedLine>& tokenBuff, size_t start, size_t end) 
{
    TokenizedLine line;

    //loop through the code starting and ending at the provided lines and store the tokenized strings in the tokenBuff
    for(size_t i=start+1; i<end-1; i++) {
        //search for assignments (equals character)
        size_t found;
        if((found = lines[i].find("=")) != std::string::npos) {
            //found an assignment line
            line.type = LineType::ASSIGN;
            line.srcLine = i;

            std::string dst = lines[i].substr(0, found);
            std::string src = lines[i].substr(found+1, lines[i].length());

            //check to see if the dst is also declaring a variable type
            found = dst.find(" ");
            if(found != std::string::npos) {
                line.type = LineType::DECLARE_ASSIGN;

                std::string type = dst.substr(0, found);
                dst = dst.substr(found+1, dst.length());

                line.assignType = type;
            }

            line.assignDst = dst;
            line.assignSrc = src;

            tokenBuff.push_back(line);
        }
        else if(true) {
            
        }
        else {
            std::cout << "\nError! Unrecognized syntax at line " << i << ". Skipping...\t";
        }

        //search for branches
        //found = lines[i]
    }
}