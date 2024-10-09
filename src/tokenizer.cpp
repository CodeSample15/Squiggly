#include "tokenizer.h"
#include "linter.h"

using namespace Tokenizer;

//init extern variables
std::vector<TokenizedLine> varsBlock_tok = std::vector<TokenizedLine>();
std::vector<TokenizedLine> startBlock_tok = std::vector<TokenizedLine>();
std::vector<TokenizedLine> mainLoop_tok = std::vector<TokenizedLine>();
std::vector<std::vector<TokenizedLine>> functions_tok = std::vector<std::vector<TokenizedLine>>();


//helper functions prototypes--------------------------------------------------------------------------------------------
//clear out the last function that was tokenized
void clearTokenized();
//find the line of code which contains a specific header (used for finding functions)
void findCode(std::string header, std::vector<std::string>& lines, size_t& start, size_t& end, bool err=false);
//for finding the scope of the code enclosed by open and close braces
void findOpenCloseBraces(std::vector<std::string>& lines, size_t& start, size_t& end);
//same thing but only searches one line instead of several
void findOpenCloseParenthesis(std::string line, size_t& start, size_t& end);
//tokenize a section starting from a start location and an end location, storing the result in tokenBuff
void tokenizeSection(std::vector<std::string>& lines, std::vector<TokenizedLine>& tokenBuff, size_t start, size_t end);
//search for functions defined by the programmer
void discoverUserFuncs(std::vector<std::string>& lines);
std::vector<std::string> userFuncNames; //this is what discoverUserFuncs is going to edit
//check for the beginning of an else/if else statement
bool checkForElse(std::vector<std::string>& lines, size_t endOfIf);
//returns the number of times a specific character appears in a line
int countNumCharacters(std::string line, char character);
//for debug purposes
void printTokenBuff(std::vector<TokenizedLine>& buffer);

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

    //find all user-defined functions
    discoverUserFuncs(lines);

    size_t start = 0;
    size_t end = 0;

    //find and tokenize all of the non optional functions in the code
    // findCode(VAR_FUNC_HEAD, lines, start, end, true);
    // tokenizeSection(lines, varsBlock_tok, start, end);

    findCode(START_FUNC_HEAD, lines, start, end, true);
    tokenizeSection(lines, startBlock_tok, start, end);

    // findCode(UPDATE_FUNC_HEAD, lines, start, end, true);
    // tokenizeSection(lines, mainLoop_tok, start, end);

    //tokenize all of the user defined functions

    //clear memory after tokenizing
    lines.clear();

    std::cout << "Done" << std::endl;
    printTokenBuff(startBlock_tok);
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
    Same thing as the findOpenCloseBraces function but searches through one line.
    This makes implementing open and close braces easier, but comes with a drawback that the
    Squiggly language can't process items that span multiple lines within parenthesis

    The refactoring of this process requires this function to be changed to cover lines and characters within lines
    and the code that calls this function has to recognize the multi-line span of code

    I would like to do this improvement in the future, but for now I just want to get this language working, so syntax will be simpler
    for the time being.
*/
void findOpenCloseParenthesis(std::string line, size_t& start, size_t& end) {
    //each opening brace will be pushed to the stack. each closing parenthesis will remove from the stack
    //once the stack is empty, the program will stop, having found the last brace to the function specified
    std::vector<char> stack;

    bool foundFirst = false; //don't start considering a stack empty until the first open parenthesis is found

    //loop through each character in the line to find an opening parenthesis
    for(size_t i=start; i<line.length(); i++) {
        if(line[i] == '(') {
            if(!foundFirst) {
                start = i; //mark the new start location
                foundFirst = true;
            }
            stack.push_back(line[i]);
        }
        else if(line[i] == ')' && foundFirst) {
            stack.erase(stack.begin() + stack.size()-1);
        }

        if(stack.size() == 0 && foundFirst) {
            end = i;
            return;
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
        if((found = lines[i].find("=")) != std::string::npos && countNumCharacters(lines[i], '=') == 1) {
            //found an assignment line
            line.type = LineType::ASSIGN;

            /*
                Assign linetype:
                    std::string assignDst;
                    std::string assignSrc;
                    std::string assignType; //DECLARE ASSIGN
            */

            line.srcLine = i;

            //may God have mercy on us all... (this is the worst looking code I've ever written)
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
        else if((found = lines[i].find("if(")) == 0) { //should be found at 0 (first thing in the string)
            line.type = LineType::BRANCH;
            line.branchLineNumTRUE = -1;
            line.branchLineNumFALSE = -1;
            line.branchLineNumELSE = -1;

            /*
                Branch linetime:
                    std::string booleanExpression;
                    size_t branchLineNumTRUE;
                    size_t branchLineNumFALSE;
                    size_t branchLineNumELSE;
            */

            size_t ifExpressStart = 0;
            size_t ifExpressEnd = 0;
            findOpenCloseParenthesis(lines[i], ifExpressStart, ifExpressEnd);
            line.booleanExpression = lines[i].substr(3, ifExpressEnd-3); //extract the boolean expression from the if statement

            //find scope of the if statement
            size_t ifStart = i;
            size_t ifEnd = 0;
            findOpenCloseBraces(lines, ifStart, ifEnd);
            line.branchLineNumTRUE = tokenBuff.size(); //the line points to where in the tokenized array to jump to

            //tokenize the lines inside of the braces and store in a temp buffer
            std::vector<TokenizedLine> tempBuff;
            tokenizeSection(lines, tempBuff, ifStart, ifEnd);

            //check to see if the branch ends or if there's an else/if-else
            if(checkForElse(lines, end))
                line.branchLineNumELSE = tokenBuff.size() + tempBuff.size();
            else
                line.branchLineNumFALSE = tokenBuff.size() + tempBuff.size();
            
            tokenBuff.push_back(line);

            //extend code that was in the tempbuffer
            for(TokenizedLine line : tempBuff) {
                tokenBuff.push_back(line);
            }

            i = ifEnd;

            //tokenize additional if/if-else statements
            // while(checkForElse()) {
                
            // }
        }
        else {
            std::cerr << "\nError! Unrecognized syntax at line \'" << lines[i] << "\'. Skipping..." << std::endl;
        }
    }
}

void discoverUserFuncs(std::vector<std::string>& lines) 
{
    userFuncNames.clear();

    for(size_t i=0; i<lines.size(); i++) {
        if(lines[i].find(VAR_FUNC_HEAD) != std::string::npos
        || lines[i].find(START_FUNC_HEAD) != std::string::npos
        || lines[i].find(UPDATE_FUNC_HEAD) != std::string::npos) {
            //line is one of the system functions, skip over this section
            size_t end = -1;
            findOpenCloseBraces(lines, i, end);

            i = end-1; //jump to new position
        }
        else if(lines[i].find("(") != std::string::npos) { //the '(' character will be present in every function declaration
            //must be a function declaration (again assuming that linter did its job)
            try {
                std::string funcName = lines[i].substr(0, lines[i].find("("));
                userFuncNames.push_back(funcName);

                //jump to end of function
                size_t end = -1;
                findOpenCloseBraces(lines, i, end);

                i = end-1; //jump to new position
            } catch (...) {
                std::cerr << "Error parsing function name: \'" << lines[i] << "\'" << std::endl;
            }
        }
    }
}

bool checkForElse(std::vector<std::string>& lines, size_t endOfIf) {
    //big long ugly boolean that checks for an else at the end of an if statement
    return lines[endOfIf-1].find("else{") != std::string::npos
            || lines[endOfIf-1].find("else if(") != std::string::npos
            || (endOfIf < lines.size() && (lines[endOfIf].find("else{") != std::string::npos
            || lines[endOfIf].find("else if{") != std::string::npos));
}

int countNumCharacters(std::string line, char character) {
    int count = 0;
    for(size_t i=0; i<line.length(); i++) {
        if(line[i] == character) {
            count++;
        }
    }
    return count;
}

void printTokenBuff(std::vector<TokenizedLine>& buffer) {
    for(size_t i=0; i<buffer.size(); i++) {
        std::cout << i << ": ";

        switch(buffer[i].type) {
            case LineType::CALL:
                std::cout << "CALL " << buffer[i].callFuncName << "(" << buffer[i].params << ")" << std::endl;
                break;
            
            case LineType::BRANCH:
                std::cout << "BRANCH " << buffer[i].booleanExpression << "\tTRUE: " << buffer[i].branchLineNumTRUE << "\tFALSE: " << buffer[i].branchLineNumFALSE << "\tELSE: " << buffer[i].branchLineNumELSE << std::endl;
                break;

            case LineType::ASSIGN:
                std::cout << "ASSIGN \'" << buffer[i].assignDst << "\' to \'" << buffer[i].assignSrc << "\'" << std::endl;
                break;

            case LineType::DECLARE:
                std::cout << "DECLARE \'" << buffer[i].assignType << " " << buffer[i].assignDst << std::endl;;
                break;

            case LineType::DECLARE_ASSIGN:
                std::cout << "DECLARE ASSIGN \'" << buffer[i].assignType << " " << buffer[i].assignDst << "\' \'" << buffer[i].assignSrc << "\'" << std::endl;
                break;

            case LineType::FUNC_NAME:
                std::cout << "FUNC " << buffer[i].funcName << std::endl;
                break;
        }
    }
}