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
//tokenize an if statement
//void tokenizeIf(std::vector<std::string>& lines, std::vector<TokenizedLine>& tokenBuff, );
//search for functions defined by the programmer
void discoverUserFuncs(std::vector<std::string>& lines);
std::vector<std::string> userFuncNames; //this is what discoverUserFuncs is going to edit
//check for the beginning of an else/if else statement
bool checkForElse(std::vector<std::string>& lines, size_t endOfIf, size_t& elseLine);
bool checkForElse(std::vector<std::string>& lines, size_t endOfIf);
//returns the number of times a specific character appears in a line
int countNumCharacters(std::string line, char character);
//for errors (exit program)
inline void tokenizerError(std::string msg);
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
        tokenizerError("Unable to find " + header + " in code!");
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
            line.assignOperator = "=";

            /*
                Assign linetype:
                    std::string assignDst;
                    std::string assignSrc;
                    std::string assignType; //DECLARE ASSIGN
                    std::string assignOperator;
            */

            //check for special assigns (+=, -=, /=, *=)
            size_t assignFound;
            size_t offset = 1; //if a special assign is found, increase this to 2 so that the source assign substring can be found
            if((assignFound = lines[i].find("+=")) != std::string::npos) {
                line.assignOperator = "+=";
                found = assignFound;
                offset = 2;
            } else if((assignFound = lines[i].find("-=")) != std::string::npos) {
                line.assignOperator = "-=";
                found = assignFound;
                offset = 2;
            } else if((assignFound = lines[i].find("/=")) != std::string::npos) {
                line.assignOperator = "/=";
                found = assignFound;
                offset = 2;
            } else if((assignFound = lines[i].find("*=")) != std::string::npos) {
                line.assignOperator = "*=";
                found = assignFound;
                offset = 2;
            }

            //some ugly code but just roll with it
            std::string dst = lines[i].substr(0, found);
            std::string src = lines[i].substr(found+offset, lines[i].length()); //here is the ONLY place offset will be used, ignore the value from this point on

            //check to see if the dst is also declaring a variable type
            found = dst.find(" ");
            if(found != std::string::npos) {
                if(line.assignOperator.compare("=") != 0) { //declarations should NOT be combined with special assigns (+=, -=, etc) since the variable won't have a value yet
                    tokenizerError("Can not perform " + line.assignOperator + " when declaring variable");
                }

                line.type = LineType::DECLARE_ASSIGN;

                std::string varType = dst.substr(0, found);
                dst = dst.substr(found+1, dst.length());

                line.assignType = varType;
            }

            line.assignDst = dst;
            line.assignSrc = src;

            tokenBuff.push_back(line);
        }
        else if((found = lines[i].find("if(")) == 0) { //should be found at 0 (first thing in the string)
            //this string parsing method is a mess but just go with it...
            line.type = LineType::BRANCH;

            /*
                Branch linetype:
                    std::string booleanExpression;
                    size_t branchLineNumTRUE;
                    size_t branchLineNumFALSE;
                    size_t branchLineNumELSE;
            */

            //TODO: FOR THE LOVE OF GOD PLEASE REFACTOR THIS CODE

            line.branchLineNumTRUE = -1;
            line.branchLineNumEND = -1;
            line.branchLineNumELSE = -1;

            size_t ifExpressStart = 0;
            size_t ifExpressEnd = 0;
            findOpenCloseParenthesis(lines[i], ifExpressStart, ifExpressEnd);
            line.booleanExpression = lines[i].substr(3, ifExpressEnd-3); //extract the boolean expression from the if statement

            //find scope of the if statement
            size_t ifStart = i;
            size_t ifEnd = 0;
            findOpenCloseBraces(lines, ifStart, ifEnd);
            line.branchLineNumTRUE = tokenBuff.size()+1; //the line points to where in the tokenized array to jump to

            //tokenize the lines inside of the braces and store in a temp buffer
            std::vector<TokenizedLine> tempBuff;
            tokenizeSection(lines, tempBuff, ifStart, ifEnd);

            //check to see if the branch ends or if there's an else/if-else
            if(checkForElse(lines, ifEnd))
                line.branchLineNumELSE = tokenBuff.size() + tempBuff.size() + 1;
            else
                line.branchLineNumEND = tokenBuff.size() + tempBuff.size() + 1;
            
            tokenBuff.push_back(line); //push back the if statement

            //extend code that was in the tempbuffer
            for(TokenizedLine line : tempBuff) {
                tokenBuff.push_back(line);
            }

            i = ifEnd;

            //tokenize additional if-else statements
            size_t elseLocation = 0; //value set within checkForElse
            while(checkForElse(lines, ifEnd, elseLocation)) {
                bool elseFound = false; //when an 'else' statement is found by itself, that's the end of the branch statement
                line.branchLineNumTRUE = -1;
                line.branchLineNumEND = -1;
                line.branchLineNumELSE = -1;

                if(lines[elseLocation].find("if(") == 5) { //if statement found on the same line as the else, immediately after the keyword "else" (6 characters)
                    //linetype should already be BRANCH still
                    //this code is basically a copy-paste from the above 'if' parsing code (yeah yeah, i know. bad practice. whatever) TODO: Refactor this code with functions to make it easier to read and understand
                    findOpenCloseParenthesis(lines[elseLocation], ifExpressStart, ifExpressEnd);
                    line.booleanExpression = lines[i].substr(8, ifExpressEnd-8); //extract the boolean expression from the if statement (8 characters for 'else if(')
                }
                else {
                    line.type = LineType::BRANCH_ELSE;
                    elseFound = true; //end of the line
                }

                ifStart = elseLocation;
                findOpenCloseBraces(lines, ifStart, ifEnd);
                line.branchLineNumTRUE = tokenBuff.size()+1;

                //tokenize code inside braces
                tempBuff.clear();
                tokenizeSection(lines, tempBuff, ifStart, ifEnd);

                //check to see if the branch ends or if there's an else/if-else
                if(checkForElse(lines, ifEnd))
                    line.branchLineNumELSE = tokenBuff.size() + tempBuff.size() + 1;
                else
                    line.branchLineNumEND = tokenBuff.size() + tempBuff.size() + 1;
                
                tokenBuff.push_back(line);

                //extend code that was in the tempbuffer
                for(TokenizedLine line : tempBuff) {
                    tokenBuff.push_back(line);
                }

                i = ifEnd;

                if(elseFound)
                    break; //all done, don't risk errors
            }

            i--;
        }
        else if((found = lines[i].find(LOOP_HEADER)) != std::string::npos) {
            line.type = LineType::LOOP;

            /*
                Loop linetype:
                    std::string loopTimes;
                    size_t loopEnd;
            */

            //find the scope of the loop
            size_t loopStart = i;
            size_t loopEnd;
            findOpenCloseBraces(lines, loopStart, loopEnd);

            line.loopStart = tokenBuff.size() + 1;

            //parse the number of loops
            if((found = lines[i].find("(")) != std::string::npos) {
                //find the index of where the loop amount substring ends
                size_t loopArgStart = found;
                size_t loopArgEnd = 0;
                findOpenCloseParenthesis(lines[i], loopArgStart, loopArgEnd);

                //read the loop times statement from the line
                line.loopTimes = lines[i].substr(found+1, (loopArgEnd - loopArgStart)-1);

                //empty parenthesis: no argument passed to the loop function
                if(line.loopTimes.length() == 0)
                    tokenizerError("No argument passed to repeat statement!");

                //tokenize the insize of the loop
                std::vector<TokenizedLine> tempBuff;
                tokenizeSection(lines, tempBuff, loopStart, loopEnd);

                //determine which instruction the loop should jump to when finished
                line.loopEnd = tempBuff.size() + tokenBuff.size() + 1;

                //push back the finished tokenized line and add the instructions stored in the temporary buffer
                tokenBuff.push_back(line);
                for(TokenizedLine line : tempBuff)
                    tokenBuff.push_back(line);
                    
                i = loopEnd;
            }
            else {
                tokenizerError("Error parsing loop statement. Incorrect format:\n'" + lines[i] + "'");
            }
        } 
        else if(lines[i].length() == 1 && (lines[i][0] == '{' || lines[i][0] == '}')) {
            continue; //ignore lines with just { or }
        }
        else {
            //will change this to a proper tokenizerError message when testing is finished
            std::cerr << "\nError! Unrecognized syntax at line \'" << lines[i] << "\'. Skipping..." << std::endl;
        }
    }
}

/*
    Loop through the untokenized lines to discover programmer-defined function heads.

    Once found, record the name of the function to the userFuncNames global vector for future
    querying.
*/
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

/*
    The two methods below search for else/else-if tokens at the end of a specific line

    The line number given should be the end line number of a if statement closing brace
*/
bool checkForElse(std::vector<std::string>& lines, size_t endOfIf, size_t& elseLine) {
    //big long ugly boolean that checks for an else at the end of an if statement
    if(lines[endOfIf-1].find("else") != std::string::npos
    || lines[endOfIf-1].find("else if(") != std::string::npos) {
        elseLine = endOfIf-1; //same line as the closing brace of the if statement
        return true;
    }

    if((endOfIf < lines.size() && (lines[endOfIf].find("else") != std::string::npos
    || lines[endOfIf].find("else if(") != std::string::npos))) {
        elseLine = endOfIf; //line after the closing brace of the if statement
        return true;
    }

    return false;
}

bool checkForElse(std::vector<std::string>& lines, size_t endOfIf) {
    //big long ugly boolean that checks for an else at the end of an if statement
    if(lines[endOfIf-1].find("else") != std::string::npos
    || lines[endOfIf-1].find("else if(") != std::string::npos) {
        return true;
    }

    if((endOfIf < lines.size() && (lines[endOfIf].find("else{") != std::string::npos
    || lines[endOfIf].find("else if(") != std::string::npos))) {
        return true;
    }

    return false;
}

/*
    Offers some basic utility to the program
*/
int countNumCharacters(std::string line, char character) {
    //count the number of times a specific character appears in a string
    int count = 0;
    for(size_t i=0; i<line.length(); i++) {
        if(line[i] == character) {
            count++;
        }
    }
    return count;
}

/*
    Simple utility for returning a non-crashing error message to the main code (will be useful when ported to different devices
    since it allows us to control how a device handles an error. For PCs, the code will simply exit)
*/
inline void tokenizerError(std::string message) {
    clearTokenized();
    throw std::runtime_error("Tokenizer Failed! : " + message);
}

/*
    This line is useful for debugging the tokenizer.
    Allows you to print out the tokenized version of a script and also demonstrated what values are used for what in the TokenizedLine struct
*/
void printTokenBuff(std::vector<TokenizedLine>& buffer) {
    for(size_t i=0; i<buffer.size(); i++) {
        std::cout << i << ": ";

        switch(buffer[i].type) {
            case LineType::CALL:
                std::cout << "CALL " << buffer[i].callFuncName << "(" << buffer[i].params << ")" << std::endl;
                break;
            
            case LineType::BRANCH:
                std::cout << "BRANCH " << buffer[i].booleanExpression << "   TRUE: " << buffer[i].branchLineNumTRUE << "   END: " << buffer[i].branchLineNumEND << "   ELSE: " << buffer[i].branchLineNumELSE << std::endl;
                break;

            case LineType::BRANCH_ELSE:
                std::cout << "ELSE END: " << buffer[i].branchLineNumEND << std::endl;
                break;

            case LineType::LOOP:
                std::cout << "LOOP " << buffer[i].loopTimes << " TIMES START=" << buffer[i].loopStart << ", END=" << buffer[i].loopEnd << std::endl;
                break;

            case LineType::ASSIGN:
                std::cout << "ASSIGN \'" << buffer[i].assignDst << "\' to \'" << buffer[i].assignSrc << "\' using '" << buffer[i].assignOperator << "'" << std::endl;
                break;

            case LineType::DECLARE:
                std::cout << "DECLARE \'" << buffer[i].assignType << " " << buffer[i].assignDst << std::endl;;
                break;

            case LineType::DECLARE_ASSIGN:
                std::cout << "DECLARE-ASSIGN \'" << buffer[i].assignType << " " << buffer[i].assignDst << "\' \'" << buffer[i].assignSrc << "\'" << std::endl;
                break;

            case LineType::FUNC_NAME:
                std::cout << "FUNC " << buffer[i].funcName << std::endl;
                break;

            default:
                std::cout << "UNDEFINED LINE" << std::endl;
                break;
        }
    }
}