#include <sstream>

#include "tokenizer.hpp"
#include "linter.hpp"
#include "built-in.hpp"

#define TOK_DEBUGGING false

using namespace Tokenizer;

//init extern variables
std::vector< std::shared_ptr<TokenizedLine> > varsBlock_tok = std::vector< std::shared_ptr<TokenizedLine> >();
std::vector< std::shared_ptr<TokenizedLine> > startBlock_tok = std::vector< std::shared_ptr<TokenizedLine> >();
std::vector< std::shared_ptr<TokenizedLine> > mainLoop_tok = std::vector< std::shared_ptr<TokenizedLine> >();
std::vector< std::vector< std::shared_ptr<TokenizedLine> > > functions_tok = std::vector<std::vector< std::shared_ptr<TokenizedLine> >>();

//for keeping track of if-statement tokenization
size_t branch_id = 0;

//helper functions prototypes--------------------------------------------------------------------------------------------

//find the line of code which contains a specific header (used for finding functions)
void findCode(std::string header, std::vector<std::string>& lines, size_t& start, size_t& end, bool err=false);
//for finding the scope of the code enclosed by open and close braces
void findOpenCloseBraces(std::vector<std::string>& lines, size_t& start, size_t& end);
//same thing but only searches one line instead of several
void findOpenCloseParenthesis(std::string line, size_t& start, size_t& end);
//same thing but for strings (ignore syntax in strings, treat them as strings)
void findOpenCloseStrings(std::string line, size_t start, size_t& end);
//tokenize a section starting from a start location and an end location, storing the result in tokenBuff
void tokenizeSection(std::vector<std::string>& lines, std::vector< std::shared_ptr<TokenizedLine> >& tokenBuff, size_t baseBuffSize, size_t start, size_t end);
//tokenize an if statement, return the end line of the if statement in the source code
size_t tokenizeIf(std::vector<std::string>& lines, std::vector< std::shared_ptr<TokenizedLine> >& tokenBuff, size_t& i, size_t baseBuffSize, bool isElseIf, size_t id);
//use the ',' delimiter to get the individual arguments passed to a function and stored them in argBuff
void parseArgsFromString(std::string s, std::vector<std::string>& argBuff);
//search for functions defined by the programmer
void discoverUserFuncs(std::vector<std::string>& lines);
std::vector<std::string> userFuncNames; //this is what discoverUserFuncs is going to edit
std::vector<std::vector<size_t>> userFuncRanges; //discoverUserFuncs will also edit this
//check for the beginning of an else/if else statement
bool checkForElse(std::vector<std::string>& lines, size_t endOfIf, size_t& elseLine);
//returns the number of times a specific character appears in a line
int countNumCharacters(std::string line, char character);
//returns the number of segments in the line (segments are sections of a string not broken up by any non-alpha character)
int numSegmentsInString(std::string line);
//find user functions in a line of code (for searching for function calls). Returns the name of the function found
std::string searchForUserFunctions(std::string line);
//find object functions. Returns the name of the object its function being called
std::string searchForObjectFunctions(std::string line);
//delete old programs (allows this code to be ran many times without exiting)
void clearTokens();
//for errors (exit program)
inline void tokenizerError(std::string msg);
//for debug purposes
void printTokenBuff(std::vector< std::shared_ptr<TokenizedLine> >& buffer);

/*
    Parse the code line by line and build a tokenized version of the script.
    Assumes that the linter did its job and the syntax of the code is correct.
    Uses the provided vector of lines to fill the tokenized vectors with TOkenizedLine structs
*/
void Tokenizer::tokenize(std::vector<std::string>& lines)
{
    BuiltIn::Print("Tokenizing code...\t", false);

    branch_id = 0; //reset branch tracking

    //delete old tokens if there are any (for when code is ported to embedded devices that shouldn't exit)
    clearTokens();

    //find all user-defined functions and place their names into userFuncNames
    discoverUserFuncs(lines);

    size_t start = 0;
    size_t end = 0;

    //find and tokenize all of the non optional functions in the code
    findCode(VAR_FUNC_HEAD, lines, start, end, true);
    tokenizeSection(lines, varsBlock_tok, 0, start, end);

    findCode(START_FUNC_HEAD, lines, start, end, true);
    tokenizeSection(lines, startBlock_tok, 0, start, end);

    findCode(UPDATE_FUNC_HEAD, lines, start, end, true);
    tokenizeSection(lines, mainLoop_tok, 0, start, end);

    //tokenize all of the user defined functions found by 'discoverUserFuncs'
    for(size_t i=0; i<userFuncNames.size(); i++) {
        std::vector< std::shared_ptr<TokenizedLine> > tokenBuffer;

        //create the header line for this function
        std::shared_ptr<FuncNameLine> titleLine = std::make_shared<FuncNameLine>(FuncNameLine());
        titleLine->type = LineType::FUNC_NAME;
        titleLine->funcName = userFuncNames.at(i);
        tokenBuffer.push_back(titleLine); //this will make it easier to find the function in the functions_tok array when the program needs to call it

        //add expected arguments to list
        std::string headerLine = lines[userFuncRanges[i][0]];

        size_t paramsStart = 0;
        size_t paramsEnd = 0;
        findOpenCloseParenthesis(headerLine, paramsStart, paramsEnd);
        if(paramsStart == 0) {
            //didn't find arguments on first line, try the line before since that's where the header might be (braces start on line after header line)
            headerLine = lines[userFuncRanges[i][0]-1]; //TODO: maybe check for possible underflow when doing this...
            findOpenCloseParenthesis(headerLine, paramsStart, paramsEnd);
        }

        parseArgsFromString(headerLine.substr(paramsStart+1, paramsEnd-paramsStart-1), titleLine->expectedArgs); //add expected arguments to function declaration header

        //tokenize the function and add to the tokenized vector
        start = userFuncRanges[i][0];
        end = userFuncRanges[i][1];

        tokenizeSection(lines, tokenBuffer, 0, start, end);
        functions_tok.push_back(tokenBuffer);
    }

    //clear memory after tokenizing
    lines.clear();

    BuiltIn::Print("Done\n");

    #if TOK_DEBUGGING
        printTokenBuff(startBlock_tok);
    #endif
}

//finds the start and end locations for code segments with specific headers (system functions called automatically)
void findCode(std::string header, std::vector<std::string>& lines, size_t& start, size_t& end, bool err) 
{
    //scan for function header
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
        bool inString = false;
        char stringType = '\0';

        //loop through each line to find an opening brace
        for(char letter : lines[i]) {
            //ignore items inside a string
            if(letter=='\'' || letter=='\"') {
                if(stringType==letter) {
                    //already in string, this is the closing string
                    inString = false;
                    stringType = '\0';
                }
                else {
                    inString = true;
                    stringType = letter;
                }
            }

            if(inString)
                continue; //ignore anything inside of the string

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
    end = start; //safe default if there is no end found to the braces
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
    std::vector<char> stack;

    bool foundFirst = false; //don't start considering a stack empty until the first open parenthesis is found

    //loop through each character in the line to find an opening parenthesis
    for(size_t i=start; i<line.length(); i++) {
        findOpenCloseStrings(line, i, i); //jump to the end of a string 

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
    Use this to prevent erros when parsing strings. Allows us to ignore characters marked as strings by the programmer 
    ("this is a test :)") <-- The ')' in this string would error out the program without this method
*/
void findOpenCloseStrings(std::string line, size_t start, size_t& end) {
    char stringMarker = line[start]; // if the programmer is using "" to represent strings, set to '"', otherwise set to '''. Used to tell whether the programmer is using "" or '' for the strings

    if(stringMarker != '\'' && stringMarker != '\"')
        return;
    
    for(size_t i=start+1; i<line.length(); i++) {
        if(line[i]==stringMarker) {
            end = i;
            return;
        }
    }
}

/*
    Go to the section of the code specified with the start and end lines variables and go
    line by line to tokenize the program into a tokenBuff
*/
void tokenizeSection(std::vector<std::string>& lines, std::vector< std::shared_ptr<TokenizedLine> >& tokenBuff, size_t baseBuffSize, size_t start, size_t end) 
{
    std::string functionName; //for detecting user function calls in a line

    //loop through the code starting and ending at the provided lines and store the tokenized strings in the tokenBuff
    for(size_t i=start+1; i<end-1; i++) {
        size_t found;

        //search for assignments (equals character)
        if((found = lines[i].find("=")) != std::string::npos && countNumCharacters(lines[i], '=') == 1 && lines[i].find("if(")!=0 && lines[i].find("else if(")!=0) {
            //found an assignment line
            std::shared_ptr<AssignLine> line = std::make_shared<AssignLine>(AssignLine());

            line->type = LineType::ASSIGN;
            line->assignOperator = "=";

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
                line->assignOperator = "+=";
                found = assignFound;
                offset = 2;
            } else if((assignFound = lines[i].find("-=")) != std::string::npos) {
                line->assignOperator = "-=";
                found = assignFound;
                offset = 2;
            } else if((assignFound = lines[i].find("/=")) != std::string::npos) {
                line->assignOperator = "/=";
                found = assignFound;
                offset = 2;
            } else if((assignFound = lines[i].find("*=")) != std::string::npos) {
                line->assignOperator = "*=";
                found = assignFound;
                offset = 2;
            }

            //some ugly code but just roll with it
            std::string dst = lines[i].substr(0, found);
            std::string src = lines[i].substr(found+offset, lines[i].length()); //here is the ONLY place offset will be used, ignore the value from this point on

            //check to see if the dst is also declaring a variable type
            found = dst.find(" ");
            if(found != std::string::npos) {
                if(line->assignOperator.compare("=") != 0) { //declarations should NOT be combined with special assigns (+=, -=, etc) since the variable won't have a value yet
                    tokenizerError("Can not perform " + line->assignOperator + " when declaring variable");
                }

                line->type = LineType::DECLARE_ASSIGN;

                std::string varType = dst.substr(0, found);
                dst = dst.substr(found+1, dst.length());

                line->assignType = varType;
            }

            line->assignDst = dst;
            line->assignSrc = src;

            tokenBuff.push_back(line);
        }
        else if((found = lines[i].find("if(")) == 0) { //should be found at 0 (first thing in the string)
            size_t ifEnd = tokenizeIf(lines, tokenBuff, i, baseBuffSize, true, ++branch_id);
            
            size_t elseLocation = 0;
            while(checkForElse(lines, ifEnd, elseLocation)) {
                if(elseLocation >= end-1)
                    break; //encountering the start of a different else statement

                i = elseLocation;
                bool elseIf = lines[i].find("if(") != std::string::npos;
                
                ifEnd = tokenizeIf(lines, tokenBuff, i, baseBuffSize, elseIf, branch_id);

                if(!elseIf) //just a regular else statement
                    break;
            }

            i = ifEnd-1;
        }
        else if((found = lines[i].find(LOOP_HEADER)) != std::string::npos || (found = lines[i].find(WHILE_LOOP_HEADER)) != std::string::npos) {
            std::shared_ptr<LoopLine> line = std::make_shared<LoopLine>(LoopLine());

            line->type = LineType::LOOP;

            /*
                Loop linetype:
                    std::string loopTimes;
                    size_t loopStart;
                    size_t loopEnd;
                    bool isWhile;
            */

            //determine if this is a while loop or a repeat statement
            line->isWhile = lines[i].find(WHILE_LOOP_HEADER) != std::string::npos;

            //find the scope of the loop
            size_t loopStart = i;
            size_t loopEnd;
            findOpenCloseBraces(lines, loopStart, loopEnd);

            line->loopStart = tokenBuff.size() + 1 + baseBuffSize;

            //parse the number of loops
            if((found = lines[i].find("(")) != std::string::npos) {
                //find the index of where the loop amount substring ends
                size_t loopArgStart = found;
                size_t loopArgEnd = 0;
                findOpenCloseParenthesis(lines[i], loopArgStart, loopArgEnd);

                //read the loop times statement from the line
                line->loopTimes = lines[i].substr(found+1, (loopArgEnd - loopArgStart)-1);

                //empty parenthesis: no argument passed to the loop function
                if(line->loopTimes.length() == 0)
                    tokenizerError("No argument passed to loop statement!");

                //tokenize the insize of the loop
                std::vector< std::shared_ptr<TokenizedLine> > tempBuff;
                tokenizeSection(lines, tempBuff, tokenBuff.size()+1, loopStart, loopEnd);

                //determine which instruction the loop should jump to when finished
                line->loopEnd = tempBuff.size() + tokenBuff.size() + 1 + baseBuffSize;

                //push back the finished tokenized line and add the instructions stored in the temporary buffer
                tokenBuff.push_back(line);
                for(std::shared_ptr<TokenizedLine> tmpLine : tempBuff)
                    tokenBuff.push_back(tmpLine);
                    
                i = loopEnd-1;
            }
            else {
                tokenizerError("Error parsing loop statement. Incorrect format:\n'" + lines[i] + "'");
            }
        }
        else if((functionName = searchForUserFunctions(lines[i])).length() != 0 || (functionName = searchForObjectFunctions(lines[i])).length() != 0) {
            std::shared_ptr<CallLine> line = std::make_shared<CallLine>(CallLine());

            line->type = LineType::CALL;

            /*
                Function call linetype:
                    std::string callFuncName;
                    std::string params;
            */

            line->callFuncName = functionName;

            size_t paramsStart = 0;
            size_t paramsEnd = 0;

            findOpenCloseParenthesis(lines[i], paramsStart, paramsEnd); //parse parameters passed to function
            parseArgsFromString(lines[i].substr(paramsStart+1, paramsEnd-paramsStart-1), line->args);

            tokenBuff.push_back(line);
        }
        else if(lines[i][0] == '^') {
            //built in function call
            std::shared_ptr<CallLine> line = std::make_shared<CallLine>(CallLine());

            line->type = LineType::BI_CALL;

            /*
                Built in function call linetype:
                    std::string callFuncName;
                    std::string params;
            */
            
            size_t paramsStart = 0;
            size_t paramsEnd = 0;

            findOpenCloseParenthesis(lines[i], paramsStart, paramsEnd);
            parseArgsFromString(lines[i].substr(paramsStart+1, paramsEnd-paramsStart-1), line->args);
            line->callFuncName = lines[i].substr(1, paramsStart-1);

            tokenBuff.push_back(line);
        }
        else if(numSegmentsInString(lines[i])==2 && lines[i].find(" ") != std::string::npos) {
            //define (only two segment of unbroken string: aka variable type and variable name)
            //Example: 'int lives' --> Define a variables called 'lives' with type 'int' (two strings separated by the non-alpha character " ")
            std::shared_ptr<DeclareLine> line = std::make_shared<DeclareLine>(DeclareLine());

            line->type = LineType::DECLARE;

            /*
                Declare linetype:
                    std::string varName;
                    std::string varType;
            */

            size_t typeStringLength = lines[i].find(" ");
            line->varType = lines[i].substr(0, typeStringLength);
            line->varName = lines[i].substr(typeStringLength+1, lines[i].length()-typeStringLength-1);

            tokenBuff.push_back(line);
        }
        else if(lines[i].length() == 1 && (lines[i][0] == '{' || lines[i][0] == '}')) {
            continue; //ignore lines with just { or }
        }
        else {
            //std::cerr << "\nError! Unrecognized syntax at line \'" << lines[i] << "\'. Skipping..." << std::endl;
            tokenizerError("Error parsing line: '" + lines[i] + "'");
        }
    }
}

/*

*/
size_t tokenizeIf(std::vector<std::string>& lines, std::vector< std::shared_ptr<TokenizedLine> >& tokenBuff, size_t& i, size_t baseBuffSize, bool isElseIf, size_t id)
{
    std::shared_ptr<BranchLine> line = std::make_shared<BranchLine>(BranchLine());

    //this string parsing method is a mess but just go with it...
    line->type = LineType::BRANCH;

    /*
        Branch linetype:
            std::string booleanExpression;
            size_t branchLineNumTRUE;
            size_t branchLineNumFALSE;
            size_t branchLineNumELSE;
    */

    line->branchLineNumTRUE = 0;
    line->branchLineNumELSE = 0;
    line->ifElse = isElseIf;
    line->id = id;

    //don't do this if this is an else statement only
    if(isElseIf) {
        size_t ifExpressStart = 0;
        size_t ifExpressEnd = 0;
        findOpenCloseParenthesis(lines[i], ifExpressStart, ifExpressEnd);
        line->booleanExpression = lines[i].substr(ifExpressStart+1, ifExpressEnd-ifExpressStart-1); //extract the boolean expression from the if statement
    } else {
        line->booleanExpression = "";
        line->type = LineType::BRANCH_ELSE;
    }

    //find scope of the if statement
    size_t ifStart = i;
    size_t ifEnd = 0;
    findOpenCloseBraces(lines, ifStart, ifEnd);
    line->branchLineNumTRUE = tokenBuff.size()+1+baseBuffSize; //the line points to where in the tokenized array to jump to if the boolean is true

    //tokenize the lines inside of the braces and store in a temp buffer
    std::vector< std::shared_ptr<TokenizedLine> > tempBuff;
    tokenizeSection(lines, tempBuff, tokenBuff.size()+1, ifStart, ifEnd);

    line->branchLineNumELSE = tokenBuff.size() + tempBuff.size() + 1 + baseBuffSize;
    
    tokenBuff.push_back(line); //push back the if statement
    
    //extend code that was in the tempbuffer
    for(std::shared_ptr<TokenizedLine> tmpLine : tempBuff) {
        tokenBuff.push_back(tmpLine);
    }

    return ifEnd;
}

/*
    Get the string parsed from the tokenizer and convert into a vector of strings
*/
void parseArgsFromString(std::string s, std::vector<std::string>& argBuff) 
{
    if(s.length()==0)
        return;

    std::string tmp = "";
    int tmpStart = 0;
    for(size_t i=0; i<s.length(); i++) {
        findOpenCloseStrings(s, i, i); //skip over strings
        if(s[i] == ',') {
            //delimiter found, section off the string and add to the buffer
            argBuff.push_back(s.substr(tmpStart, i-tmpStart));
            tmpStart = i+1;
        }
    }

    argBuff.push_back(s.substr(tmpStart, s.length()-tmpStart));
}

/*
    Loop through the untokenized lines to discover programmer-defined function heads.

    Once found, record the name of the function to the userFuncNames global vector for future
    querying.
*/
void discoverUserFuncs(std::vector<std::string>& lines) 
{
    userFuncNames.clear();
    userFuncRanges.clear();

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

                std::vector<size_t> range;
                range.push_back(i);
                range.push_back(end);
                userFuncRanges.push_back(range);

                i = end-1; //jump to new position
            } catch (...) {
                tokenizerError("Error parsing function name: \'" + lines[i] + "\'");
            }
        }
    }
}

/*
    The two methods below search for else/else-if tokens at the end of a specific line

    The line number given should be the end line number of a if statement closing brace
*/
bool checkForElse(std::vector<std::string>& lines, size_t endOfIf, size_t& elseLine) {

    //checks for an else at the end of an if statement
    if(lines[endOfIf-1].find("else") == 0 || lines[endOfIf-1].find("else") == 1) {
        elseLine = endOfIf-1; //same line as the closing brace of the if statement
        return true;
    }

    if(endOfIf < lines.size() && (lines[endOfIf].find("else") == 0 || lines[endOfIf].find("else") == 1)) {
        elseLine = endOfIf; //line after the closing brace of the if statement
        return true;
    }

    return false;
}

/*
    Allows the tokenizer to find user defined function calls
    Input is a line to search, and the output is the name of the user defined function that is being called in the line

    As of right now, Squiggly does not support return statements in functions, meaning for functions to change values in one part of code, they have to
    use global variables.

    Because of this, searching for functions is easier since there should be no reason that the location of the function call is anywhere but
    the first character of a line. In the future, this will have to change to allow function calls to be detected from anywhere in a line, treated
    as a variable, and evaluated to a value if it returns something. Right now this allows the development of Squiggly to be sped up slightly, but
    in a future update I'd like to revisit this approach
*/
std::string searchForUserFunctions(std::string line) {
    for(std::string funName : userFuncNames) {
        if(line.find(funName + "(") == 0) {
            return funName;
        }
    }

    return ""; //return an empty string otherwise
}

std::string searchForObjectFunctions(std::string line) {
    size_t dotLocation = line.find(".");
    size_t paramsLocation = line.find("(");
    if(dotLocation != std::string::npos && paramsLocation != std::string::npos && paramsLocation > dotLocation)
        return line.substr(0, paramsLocation);

    return "";
}

/*
    Clearing out old tokens
*/
void clearTokens() {
    varsBlock_tok.clear();
    startBlock_tok.clear();
    mainLoop_tok.clear();
    functions_tok.clear();
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

int numSegmentsInString(std::string line) {
    int count = 0;
    bool alphaFound = false;
    for(size_t i=0; i<line.length(); i++) {
        if(std::isalpha(line[i]) && !alphaFound) {
            //found a segment of characters unbroken by non-alpha characters
            alphaFound = true;
            count++;
        }
        else if(!std::isalpha(line[i]) && line[i]!='_' && line[i]!='[' && line[i]!=']') { //_ is used to join strings, treat as a string not a delimiter. Brackets are also on the ignore list to allow the creation of array declarations. Since this method is only used (as of right now) for detecting declaration statements, I see no problem with adding this edge case
            //found a delimiter (non-alpha character)
            alphaFound = false;
        }
    }

    return count;
}

/*
    Simple utility for returning a non-crashing error message to the main code (will be useful when ported to different devices
    since it allows us to control how a device handles an error. For PCs, the code will simply exit)
*/
inline void tokenizerError(std::string message) {
    throw std::runtime_error("Tokenizer Failed! : " + message);
}

/*
    This line is useful for debugging the tokenizer.
    Allows you to print out the tokenized version of a script and also demonstrated what values are used for what in the TokenizedLine struct

    USE THIS AS A REFERENCE FOR HOW THE TOKENS SHOULD BE PARSED
*/
void printTokenBuff(std::vector< std::shared_ptr<TokenizedLine> >& buffer) {
    //for casting the TokenizedLine into its childrens' forms
    CallLine* callLine;
    BranchLine* branchLine;
    LoopLine* loopLine;
    AssignLine* assignLine;
    DeclareLine* declareLine;
    FuncNameLine* funcNameLine;
    
    for(size_t i=0; i<buffer.size(); i++) {
        std::stringstream ss;
        ss << i << ": ";

        switch(buffer[i]->type) {
            case LineType::CALL:
                callLine = (CallLine*)buffer[i].get();
                ss << "CALL " << callLine->callFuncName << "(";
                for(size_t i=0; i<callLine->args.size(); i++)
                    ss << callLine->args[i] << ", ";
                ss << ")";
                BuiltIn::Print(ss.str());
                break;
            
            case LineType::BI_CALL:
                callLine = (CallLine*)buffer[i].get();
                ss << "BUILT-IN CALL " << callLine->callFuncName << "(";
                for(size_t i=0; i<callLine->args.size(); i++)
                    ss << callLine->args[i] << ", ";
                ss << ")";
                BuiltIn::Print(ss.str());
                break;
            
            case LineType::BRANCH:
                branchLine = (BranchLine*)buffer[i].get();
                ss << "BRANCH " << branchLine->booleanExpression << " (ID: " << branchLine->id << ")   TRUE: " << branchLine->branchLineNumTRUE << "   " << (branchLine->ifElse ? "IF ELSE: " : "ELSE ") << branchLine->branchLineNumELSE;
                BuiltIn::Print(ss.str());
                break;

            case LineType::BRANCH_ELSE:
                branchLine = (BranchLine*)buffer[i].get();
                ss << "ELSE END: " << branchLine->branchLineNumELSE << " (ID: " << branchLine->id << ")";
                BuiltIn::Print(ss.str());
                break;

            case LineType::LOOP:
                loopLine = (LoopLine*)buffer[i].get();
                if(loopLine->isWhile)
                    ss << "WHILE (" << loopLine->loopTimes << "): START=" << loopLine->loopStart << ", END=" << loopLine->loopEnd;
                else
                    ss << "LOOP (" << loopLine->loopTimes << ") TIMES: START=" << loopLine->loopStart << ", END=" << loopLine->loopEnd;
                BuiltIn::Print(ss.str());
                break;

            case LineType::ASSIGN:
                assignLine = (AssignLine*)buffer[i].get();
                ss << "ASSIGN \'" << assignLine->assignDst << "\' to \'" << assignLine->assignSrc << "\' using '" << assignLine->assignOperator << "'";
                BuiltIn::Print(ss.str());
                break;

            case LineType::DECLARE:
                declareLine = (DeclareLine*)buffer[i].get();
                ss << "DECLARE {" << declareLine->varName << "} type: " << declareLine->varType;
                BuiltIn::Print(ss.str());
                break;

            case LineType::DECLARE_ASSIGN:
                assignLine = (AssignLine*)buffer[i].get();
                ss << "DECLARE-ASSIGN \'" << assignLine->assignType << " " << assignLine->assignDst << "\' \'" << assignLine->assignSrc << "\'";
                BuiltIn::Print(ss.str());
                break;

            case LineType::FUNC_NAME:
                funcNameLine = (FuncNameLine*)buffer[i].get();
                ss << "FUNCTION " << funcNameLine->funcName << "(";
                for(std::string arg : funcNameLine->expectedArgs)
                    ss << arg << ",";
                ss << "):";
                BuiltIn::Print(ss.str());
                break;

            default:
                ss << "UNDEFINED LINE";
                BuiltIn::Print(ss.str());
                break;
        }
    }
}