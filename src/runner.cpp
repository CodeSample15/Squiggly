#include <string>
#include <memory>
#include <chrono>
#include <stdexcept>

#include "runner.hpp"
#include "built-in.hpp"
#include "linter.hpp"
#include "screen.hpp"
#include "tokenizer.hpp"
#include "utils.hpp"
#include "frontend.hpp"

using namespace Runner;

typedef std::shared_ptr<Tokenizer::TokenizedLine> TOKENIZED_PTR;

//virtual memory buffers
std::vector<Utils::SVariable> gVars;    //global variables
std::vector<Utils::SVariable> sVars;    //stack variables
std::vector<Utils::SVariable> bVars;    //built-in variables

size_t currStackFrame = 0; //points to where the program is currently using as a stack frame in sVars

//called by the program while executing a script to set all the built in Squiggly variables
void setBIVars();

//values used for built in variables
std::chrono::steady_clock::time_point lastLoopTime = std::chrono::steady_clock::now();

//useful functions
void runProgram(std::vector<TOKENIZED_PTR>& tokens, std::vector<Utils::SVariable>& memory, size_t stackFrameIdx, bool clearStackWhenDone=true, size_t startIdx=0, size_t endIdx=0, bool isFunction=false); //general function for running blocks of code
void runUserFunction(std::string name, std::vector<std::string>& args); //find a user defined function and run
void runObjectFunction(std::string name, std::vector<std::string>& args, size_t& dotLocation);
void setVariable(const std::shared_ptr<void>& dst, const std::shared_ptr<void>& src, Utils::VarType type, std::string assignType="="); //assign one value to another value
void createVariable(std::vector<Utils::SVariable>& memory, std::string name, Utils::VarType type, std::shared_ptr<void> ptr); //quick shortcut for adding a new variable to memory
void executeBranch(std::vector<TOKENIZED_PTR>& tokens, std::vector<Utils::SVariable>& memory, size_t stackFrameIdx, size_t& prgCounter);
int parseArrayDecl(std::string& name); //returns the size of the initialized array
void throwRunnerError(std::string message); //throw a runner error

//useful for debugging to have these run functions separated:

//THIS FUNCTION HAS TO RUN BEFORE SQUIGGLY CAN CORRECTLY PARSE PROGRAMS
void Runner::executeVars() { 
    //constant variables that the whole program will have access to
    createVariable(gVars, "true", Utils::VarType::BOOL, Utils::createSharedPtr(true));
    createVariable(gVars, "false", Utils::VarType::BOOL, Utils::createSharedPtr(false));

    //useful variables to replace with their own words when parsing equations. Allows exprtk to be able to run certain functions and parse boolean operations, giving Squiggly a major boost in functionality
    createVariable(gVars, "and", Utils::VarType::STRING, Utils::createSharedPtr((std::string)"and"));
    createVariable(gVars, "or", Utils::VarType::STRING, Utils::createSharedPtr((std::string)"or"));
    createVariable(gVars, "not", Utils::VarType::STRING, Utils::createSharedPtr((std::string)"not"));
    createVariable(gVars, "xor", Utils::VarType::STRING, Utils::createSharedPtr((std::string)"xor"));
    createVariable(gVars, "sqrt", Utils::VarType::STRING, Utils::createSharedPtr((std::string)"sqrt"));
    createVariable(gVars, "ceil", Utils::VarType::STRING, Utils::createSharedPtr((std::string)"ceil"));
    createVariable(gVars, "cos", Utils::VarType::STRING, Utils::createSharedPtr((std::string)"cos"));
    createVariable(gVars, "sin", Utils::VarType::STRING, Utils::createSharedPtr((std::string)"sin"));
    createVariable(gVars, "tan", Utils::VarType::STRING, Utils::createSharedPtr((std::string)"tan"));
    createVariable(gVars, "abs", Utils::VarType::STRING, Utils::createSharedPtr((std::string)"abs"));

    //built in values that the user can access (will never be cleared from virtual memory)
    createVariable(bVars, JOYSTICK_X_VAR_NAME, Utils::VarType::FLOAT, Utils::createSharedPtr((float)0.0));
    createVariable(bVars, JOYSTICK_Y_VAR_NAME, Utils::VarType::FLOAT, Utils::createSharedPtr((float)0.0));
    createVariable(bVars, BUTTON_A_VAR_NAME, Utils::VarType::BOOL, Utils::createSharedPtr(false));
    createVariable(bVars, BUTTON_B_VAR_NAME, Utils::VarType::BOOL, Utils::createSharedPtr(false));

    createVariable(bVars, FPS_VAR_NAME, Utils::VarType::INTEGER, Utils::createSharedPtr((int)0));
    createVariable(bVars, DTIME_VAR_NAME, Utils::VarType::FLOAT, Utils::createSharedPtr((float)0.0));

    createVariable(bVars, SCREEN_WIDTH_VAR_NAME, Utils::VarType::INTEGER, Utils::createSharedPtr((int)SCREEN_WIDTH));
    createVariable(bVars, SCREEN_HEIGHT_VAR_NAME, Utils::VarType::INTEGER, Utils::createSharedPtr((int)SCREEN_HEIGHT));

    //flags for built in functions to set
    createVariable(bVars, COLLISION_FLAG_VAR_NAME, Utils::VarType::BOOL, Utils::createSharedPtr(false));
    createVariable(bVars, FLOAT_RETURN_BUCKET_VAR_NAME, Utils::VarType::FLOAT, Utils::createSharedPtr((float)0.0));
    createVariable(bVars, INT_RETURN_BUCKET_VAR_NAME, Utils::VarType::INTEGER, Utils::createSharedPtr((int)0));

    //set up built-in function pointers for quick access to built-in variables
    BuiltIn::fetchBuiltInReturnVariables();

    //run global variable section of the Squiggly code and add created variables to global scope (gVars)
    runProgram(varsBlock_tok, gVars, gVars.size(), false, 0, 0, true);
}

void Runner::executeStart() { runProgram(startBlock_tok, sVars, 0, true, 0, 0, true); }
void Runner::executeUpdate() { runProgram(mainLoop_tok, sVars, 0, true, 0, 0, true); }

void Runner::flushMem() {
    gVars.clear();
    sVars.clear();
    bVars.clear();
}

bool runningProgram = false;

//main execute function with loop
void Runner::execute() 
{
    flushMem();

    executeVars();
    executeStart();

    Frontend::init();

    runningProgram = true;
    while(runningProgram) {
        #if BUILD_FOR_RASPI
        Frontend::updateReadings(); //need to manually pull values from USB Gamepad into memory
        #endif

        screen.clear(); //clear the current screen
        setBIVars(); //set built in variables

        executeUpdate();
        Frontend::drawScreen();

        if(Frontend::getExitBtn())
            runningProgram = false;
    }

    flushMem(); //avoid memory leaks
    Frontend::cleanUp();
}

Utils::SVariable* Runner::fetchVariable(std::string name, bool allowArrays) 
{
    if(name.length()<1)
        return nullptr;

    std::string memberName = "";
    size_t splitLocation = 0;
    if((splitLocation = name.find(".")) != std::string::npos) {
        //split the name of the variable name and member name of the object
        memberName = name.substr(splitLocation+1, name.length()-(splitLocation+1));
        name = name.substr(0, splitLocation);
    }
    
    int arrIndex = -1; //for use with array variables
    //check if this is an array index and parse the correct name
    if(name.find("[") != std::string::npos)
        arrIndex = parseArrayDecl(name);

    if(name[0] == BUILT_IN_VAR_PREFIX) {
        name = name.substr(1, name.length()-1); //get rid of the prefix before searching

        for(size_t i=0; i<bVars.size(); i++) {
            if(bVars[i].name == name)
                return &bVars[i];
        }
    }
    else {
        Utils::SVariable* tmp = nullptr;

        //search local stack frame first (start at most recently pushed variable and work backwards, hopefully should add some efficiency)
        if(sVars.size()>0) {
            for(size_t i=sVars.size()-1; i>=currStackFrame; i--) { //only look for variables within the current stack frame
                if(sVars[i].name == name) {
                    tmp = &sVars[i];
                    break;
                }

                if(i==0) //prevent underflow
                    break;
            }
        }

        //search global variables
        for(Utils::SVariable& var : gVars) {
            if(var.name == name) {
                tmp = &var;
                break;
            }
        }

        //variable fetched is array, return the SVariable at the requested array index
        if(tmp && tmp->isArray) {
            if(allowArrays) //ONLY IF ALLOWED: return the array object and not an indexed Squiggly variable
                return tmp;

            if(arrIndex<0 || arrIndex>=tmp->arrSize)
                throwRunnerError("Array index [" + std::to_string(arrIndex) + "] out of range for array: " + tmp->name);

            try {
                //good lord, where do I start on this one-liner
                //basically, this line casts the shared pointer into a vector of squiggly variables (which should be what the SVariable pointer is pointing to if it is marked as an array)
                //and then gets the location of the Squiggly variable at the parsed array index. That location is then stored in the tmp pointer to be returned to whatever part of the program requested it.
                //anyway, here it is:
                tmp = &((std::vector<Utils::SVariable>*)tmp->ptr.get())->at(arrIndex);

            } catch(const std::exception& e) {
                //if the above code fails in anyway, catch the error and let the programmer know they messed up
                throwRunnerError("Variable \"" + tmp->name + "\" was marked as array, but was unable to be dereferenced properly!");
            }
        } else if(tmp && arrIndex != -1) {
            //variable was indexed as an array but is not an array
            throwRunnerError("Variable \"" + tmp->name + "\" is not an array!");
        }

        if(tmp && tmp->type==Utils::VarType::OBJECT && memberName!="") {
            tmp = ((BuiltIn::Object*)tmp->ptr.get())->fetchVariable(memberName);
        }
        
        return tmp;
    }

    return nullptr; //no variable was found, return a nullptr as a safety guard
}

/*
    Execute a block of the program.

    Starts from a particular location inside the given block of tokens, and ends at either the end of the token block if endIdx is 0, or at 
    position endIdx if the value is not 0
*/
void runProgram(std::vector<TOKENIZED_PTR>& tokens, std::vector<Utils::SVariable>& memory, size_t stackFrameIdx, bool clearStackWhenDone, size_t startIdx, size_t endIdx, bool isFunction) 
{
    endIdx = endIdx==0 ? tokens.size() : endIdx;

    Tokenizer::CallLine* callLine;
    Tokenizer::LoopLine* loopLine;
    Tokenizer::AssignLine* assignLine;
    Tokenizer::DeclareLine* declareLine;

    Utils::SVariable* varBuff;
    Utils::SVariable newVariableHolder;

    int intBuff = 0; // a place for the switch statement to throw integer values into temporarily
    bool boolBuff = false;
    size_t sizeBuff = 0;
    
    //iterate through the program
    for(size_t prgCounter = startIdx; prgCounter < endIdx; prgCounter++) {
        if(isFunction)
            currStackFrame = stackFrameIdx; //update the stack frame so the program has access to only variables in the scope of the function

        TOKENIZED_PTR line = tokens[prgCounter];

        //execute instruction depending on what type of line is next in the buffer
        switch(line->type) {
            case Tokenizer::LineType::CALL:
                callLine = (Tokenizer::CallLine*)line.get();
                if((sizeBuff=callLine->callFuncName.find(".")) != std::string::npos)
                    runObjectFunction(callLine->callFuncName, callLine->args, sizeBuff);
                else
                    runUserFunction(callLine->callFuncName, callLine->args);
                break;

            case Tokenizer::LineType::BI_CALL:
                callLine = (Tokenizer::CallLine*)line.get();
                BuiltIn::runFunction(callLine->callFuncName, callLine->args);
                break;

            case Tokenizer::LineType::BRANCH:
                executeBranch(tokens, memory, memory.size(), prgCounter);
                break;

            case Tokenizer::LineType::LOOP:
                loopLine = (Tokenizer::LoopLine*)line.get();

                if(loopLine->isWhile) {
                    //run while loop
                    boolBuff = *((bool*)Utils::convertToVariable(loopLine->loopTimes, Utils::VarType::BOOL).ptr.get());

                    while(boolBuff) {
                        runProgram(tokens, memory, memory.size(), true, loopLine->loopStart, loopLine->loopEnd);
                        boolBuff = *((bool*)Utils::convertToVariable(loopLine->loopTimes, Utils::VarType::BOOL).ptr.get());
                    }
                }
                else {
                    //run repeat loop (evaluate loopTimes once)
                    intBuff = *((int*)Utils::convertToVariable(loopLine->loopTimes, Utils::VarType::INTEGER).ptr.get());

                    for(int i=0; i<intBuff; i++)
                        runProgram(tokens, memory, memory.size(), true, loopLine->loopStart, loopLine->loopEnd);
                }
                //jump to end of loop
                prgCounter = loopLine->loopEnd-1;
                break;

            case Tokenizer::LineType::ASSIGN:
                assignLine = (Tokenizer::AssignLine*)line.get();
                varBuff = fetchVariable(assignLine->assignDst);

                if(varBuff)
                    setVariable(varBuff->ptr, Utils::convertToVariable(assignLine->assignSrc, varBuff->type).ptr, varBuff->type, assignLine->assignOperator);
                else 
                    throwRunnerError("Unable to find variable '" + assignLine->assignDst + "'");
                break;

            case Tokenizer::LineType::DECLARE:
                declareLine = (Tokenizer::DeclareLine*)line.get();
                if(fetchVariable(declareLine->varName))
                    throwRunnerError("Variable '" + declareLine->varName + "' is already defined!");

                if(declareLine->varName.find("[")==std::string::npos) {
                    //create a normal variable
                    newVariableHolder.name = declareLine->varName;
                    newVariableHolder.type = Utils::stringToVarType(declareLine->varType);
                    newVariableHolder.ptr = Utils::createEmptyShared(newVariableHolder.type);
                    newVariableHolder.isArray = false;
                } else {
                    //create an array
                    std::string tmpName = declareLine->varName;
                    int arrSize = parseArrayDecl(tmpName);

                    //make sure arrSize is valid
                    if(arrSize <= 0)
                        throwRunnerError("Cannot create an array with size 0 or less! Parsed array size '" + std::to_string(arrSize) + "' from '" + tmpName + "'");

                    newVariableHolder.name = tmpName;
                    newVariableHolder.type = Utils::stringToVarType(declareLine->varType);
                    newVariableHolder.ptr = Utils::createEmptyShared(newVariableHolder.type, arrSize);
                    newVariableHolder.isArray = true;
                    newVariableHolder.arrSize = arrSize;
                }

                memory.push_back(newVariableHolder); //push new variable to stack
                break;

            case Tokenizer::LineType::DECLARE_ASSIGN:
                assignLine = (Tokenizer::AssignLine*)line.get();
                if(fetchVariable(assignLine->assignDst))
                    throwRunnerError("Variable '" + assignLine->assignDst + "' is already defined!");

                if(assignLine->assignDst.find("[")==std::string::npos) {
                    newVariableHolder.name = assignLine->assignDst;
                    newVariableHolder.type = Utils::stringToVarType(assignLine->assignType);
                    newVariableHolder.ptr = Utils::createEmptyShared(newVariableHolder.type);
                    newVariableHolder.isArray = false;
                } else {
                    throwRunnerError("Squiggly does not yet support assign-initialization of arrays! Please initialize each value in array '" + assignLine->assignDst + "' with a loop"); //this is just from laziness/lack of time :/
                }

                setVariable( newVariableHolder.ptr, 
                        Utils::convertToVariable(assignLine->assignSrc, newVariableHolder.type).ptr, 
                        newVariableHolder.type, 
                        assignLine->assignOperator );
                
                memory.push_back(newVariableHolder); //push new variable to stack
                break;

            default:
                throwRunnerError("Unknown line encountered!");
                break;
        }
    }

    //clear out stack frame
    if(clearStackWhenDone) {
        size_t numVars = memory.size() - stackFrameIdx; //number of variables created in this stack frame
        for(size_t i=0; i<numVars; i++) {
            memory.erase(memory.begin() + stackFrameIdx);
        }
    }
}

/*
    Search the tokenized user functions for a function call name
*/
void runUserFunction(std::string name, std::vector<std::string>& args) {
    Tokenizer::FuncNameLine* tmp;
    
    //search for a function with the called-for name
    bool found = false;
    for(std::vector<TOKENIZED_PTR>& function : functions_tok) {
        if(function[0]->type == Tokenizer::LineType::FUNC_NAME) {
            tmp = (Tokenizer::FuncNameLine*)function[0].get(); //cast into func name line pointer

            //compare name of function with name of function being called
            if(tmp->funcName == name) {
                found = true;
                
                //check to make sure arguments passed are correct
                if(args.size() != tmp->expectedArgs.size())
                    throwRunnerError("Unexpected number of arguments passed to function " + name + ". Got " + std::to_string(args.size()) + " expected " + std::to_string(tmp->expectedArgs.size()));
                
                //create a virtual stack frame
                int prevStackFrame = sVars.size();

                //add arguments to stack
                for(size_t i=0; i<args.size(); i++) {
                    //get variable's new name from the function parameter vector
                    std::string expected = tmp->expectedArgs[i];
                    size_t spaceLocation = expected.find(" ");
                    if(spaceLocation == std::string::npos)
                        throwRunnerError("Improper parameter declaration of function " + name); //quick error check because I don't trust the Linter

                    //extract the expected name and type
                    std::string etype = expected.substr(0, spaceLocation);
                    std::string ename = expected.substr(spaceLocation+1, expected.length()-spaceLocation-1);

                    Utils::SVariable nextVar;

                    if(ename.find("[]") != std::string::npos) {
                        //variable is expected to be an array
                        nextVar = *fetchVariable(args[i], true);

                        //get rid of brackets in ename
                        size_t bracket_loc = ename.find('[');
                        nextVar.name = ename.substr(0, bracket_loc);

                        if(!nextVar.isArray)
                            throwRunnerError("Expected \"" + args[i] + "\" to be an array, but it isn't.");
                    }
                    else {
                        nextVar = Utils::convertToVariable(args[i], Utils::stringToVarType(etype));
                        nextVar.name = ename; //split expected arg into type and name, get the name
                    }

                    sVars.push_back(nextVar); //push to stack
                }

                runProgram(function, sVars, prevStackFrame, true, 1, 0, true);
                break;
            }
        } else {
            throwRunnerError("Tokenizer screwed up function tokenizing!"); //idealy, the programmer should NEVER get this error, I'm just putting this here to call out Squiggly development screw ups
        }
    }

    if(!found)
        throwRunnerError("Cannot find function named " + name);
}

/*
    Search for defined objects and call their functions
*/
void runObjectFunction(std::string name, std::vector<std::string>& args, size_t& dotLocation) 
{
    Utils::SVariable* objectVar = fetchVariable(name.substr(0, dotLocation));
    if(objectVar) {
        if(objectVar->type != Utils::VarType::OBJECT)
            throwRunnerError("Cannot run '" + name + "' on a non object type!");

        std::string functionName = name.substr(dotLocation+1, name.length()-(dotLocation+1));
        ((BuiltIn::Object*)objectVar->ptr.get())->callFunction(functionName, args);
    } else {
        throwRunnerError("Unable to execute function '" + name + "'");
    }
}

/*
    Assigns a value to a variable using a specific assignType if necessary
    Casts the void pointers passed into the expected datatype pointers and dereferences
*/
void setVariable(const std::shared_ptr<void>& dst, const std::shared_ptr<void>& src, Utils::VarType type, std::string assignType) {
    if(dst == nullptr || src == nullptr)
        throwRunnerError("Error setting variable with nullptr!"); //yet another error I don't think squiggly users should come across, this is for developers of this project only

    switch(type) {
        case Utils::VarType::STRING:
            if(assignType=="=")
                *((std::string*)dst.get()) = *((std::string*)src.get());
            else if(assignType=="+=")
                *((std::string*)dst.get()) += *((std::string*)src.get());
            else
                throwRunnerError("Invalid assign operator '" + assignType + "' for string type");
            break;

        case Utils::VarType::INTEGER:
            if(assignType=="=")
                *((int*)dst.get()) = *((int*)src.get());
            else if(assignType=="+=")
                *((int*)dst.get()) += *((int*)src.get());
            else if(assignType=="-=")
                *((int*)dst.get()) -= *((int*)src.get());
            else if(assignType=="*=")
                *((int*)dst.get()) *= *((int*)src.get());
            else if(assignType=="/=")
                *((int*)dst.get()) /= *((int*)src.get());
            else
                throwRunnerError("Invalid assign operator '" + assignType + "' for string type");
            break;

        case Utils::VarType::DOUBLE:
            if(assignType=="=")
                *((double*)dst.get()) = *((double*)src.get());
            else if(assignType=="+=")
                *((double*)dst.get()) += *((double*)src.get());
            else if(assignType=="-=")
                *((double*)dst.get()) -= *((double*)src.get());
            else if(assignType=="*=")
                *((double*)dst.get()) *= *((double*)src.get());
            else if(assignType=="/=")
                *((double*)dst.get()) /= *((double*)src.get());
            else
                throwRunnerError("Invalid assign operator '" + assignType + "' for string type");
            break;

        case Utils::VarType::FLOAT:
            if(assignType=="=")
                *((float*)dst.get()) = *((float*)src.get());
            else if(assignType=="+=")
                *((float*)dst.get()) += *((float*)src.get());
            else if(assignType=="-=")
                *((float*)dst.get()) -= *((float*)src.get());
            else if(assignType=="*=")
                *((float*)dst.get()) *= *((float*)src.get());
            else if(assignType=="/=")
                *((float*)dst.get()) /= *((float*)src.get());
            else
                throwRunnerError("Invalid assign operator '" + assignType + "' for string type");
            break;

        case Utils::VarType::BOOL:
            if(assignType=="=")
                *((float*)dst.get()) = *((float*)src.get());
            else
                throwRunnerError("Invalid assign operator '" + assignType + "' for string type");
            break;

        default:
            throwRunnerError("Error setting variable (This is a problem with Squiggly, not with your Squiggly code. Please report this issue on the project's GitHub)");
            break;
    }
}


/*
    This basically just acts as a wrapper constructor for creating new variables easily (from the programmer's perspective)
*/
void createVariable(std::vector<Utils::SVariable>& memory, std::string name, Utils::VarType type, std::shared_ptr<void> ptr) 
{
    Utils::SVariable temp;
    temp.name = name;
    temp.type = type;
    temp.ptr = ptr;
    temp.isArray = false;

    memory.push_back(temp);
}

/*
    Tranverse a branching statement in the Squiggly code.

    Once an if statement is evaluated to true, skip to the end of the branch statement.
    If no if statement is evaluated to true, check to see if the statement has an 'else' section and execute that.
    Otherwise, skip to the end of the branching statement and return control to the calling function (runProgram)
*/
void executeBranch(std::vector<TOKENIZED_PTR>& tokens, std::vector<Utils::SVariable>& memory, size_t stackFrameIdx, size_t& prgCounter) 
{
    Tokenizer::BranchLine* branchLine;
    int intBuff = 0;
    bool executedSection = false;
    bool outOfBranch = false;
    bool expectIfElse = false;

    for(; prgCounter<tokens.size(); prgCounter++) {
        TOKENIZED_PTR line = tokens[prgCounter];

        switch(line->type) {
            case Tokenizer::LineType::BRANCH:
                branchLine = (Tokenizer::BranchLine*)line.get();

                if(expectIfElse && !branchLine->ifElse) { // if expecting an if else/else and the program reads just an if, the program has reached the beginning of a new branching statement, exit this loop
                    outOfBranch = true;
                    break;
                }

                expectIfElse = true; //from here on out, expect if else statements, an else statement, or an end to the branch

                if(!executedSection) { //only bother evaluating if no other branch of the statement was ran
                    intBuff = *((int*)Utils::convertToVariable(branchLine->booleanExpression, Utils::VarType::INTEGER).ptr.get());
                    if(intBuff) {
                        //run section inside of code
                        executedSection = true;
                        
                        runProgram(tokens, memory, stackFrameIdx, true, branchLine->branchLineNumTRUE, branchLine->branchLineNumELSE);
                    }
                }

                prgCounter = branchLine->branchLineNumELSE-1;
                break;

            case Tokenizer::LineType::BRANCH_ELSE:
                branchLine = (Tokenizer::BranchLine*)line.get();

                if(!executedSection)
                    runProgram(tokens, memory, stackFrameIdx, true, branchLine->branchLineNumTRUE, branchLine->branchLineNumELSE);

                prgCounter = branchLine->branchLineNumELSE-1;
                outOfBranch = true; //make sure the next if statement that gets picked up by the program is treated as a separate branch statement
                break;

            default:
                outOfBranch = true; //no longer looking at code from the branch section
                prgCounter--;
                break;
        }

        if(outOfBranch)
            break;
    }
}

/*
    Set all of the built in variable values that Squiggly programmers will have access to
*/
void setBIVars() {
    //input control
    std::string temp = JOYSTICK_X_VAR_NAME;
    temp.insert(0, 1, BUILT_IN_VAR_PREFIX);
    setVariable(fetchVariable(temp)->ptr, 
                Utils::createSharedPtr((float)Frontend::getHorAxis()), 
                Utils::VarType::FLOAT, "=");

    temp = JOYSTICK_Y_VAR_NAME;
    temp.insert(0, 1, BUILT_IN_VAR_PREFIX);
    setVariable(fetchVariable(temp)->ptr, 
                Utils::createSharedPtr((float)Frontend::getVertAxis()), 
                Utils::VarType::FLOAT, "=");

    temp = BUTTON_A_VAR_NAME;
    temp.insert(0, 1, BUILT_IN_VAR_PREFIX);
    setVariable(fetchVariable(temp)->ptr, 
                Utils::createSharedPtr(Frontend::getABtn()), 
                Utils::VarType::BOOL, "=");

    temp = BUTTON_B_VAR_NAME;
    temp.insert(0, 1, BUILT_IN_VAR_PREFIX);
    setVariable(fetchVariable(temp)->ptr, 
                Utils::createSharedPtr(Frontend::getBBtn()), 
                Utils::VarType::BOOL, "=");

    //other miscellaneous values
    float dtime = (float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastLoopTime).count();
    int fps = (int)(1000/dtime);
    lastLoopTime = std::chrono::steady_clock::now();

    temp = FPS_VAR_NAME;
    temp.insert(0, 1, BUILT_IN_VAR_PREFIX);
    setVariable(fetchVariable(temp)->ptr, 
                Utils::createSharedPtr(fps),
                Utils::VarType::INTEGER, "=");

    temp = DTIME_VAR_NAME;
    temp.insert(0, 1, BUILT_IN_VAR_PREFIX);
    setVariable(fetchVariable(temp)->ptr, 
                Utils::createSharedPtr(dtime/1000),
                Utils::VarType::FLOAT, "=");

    temp = SCREEN_WIDTH_VAR_NAME;
    temp.insert(0, 1, BUILT_IN_VAR_PREFIX);
    setVariable(fetchVariable(temp)->ptr, 
                Utils::createSharedPtr(SCREEN_WIDTH),
                Utils::VarType::INTEGER, "=");

    temp = SCREEN_HEIGHT_VAR_NAME;
    temp.insert(0, 1, BUILT_IN_VAR_PREFIX);
    setVariable(fetchVariable(temp)->ptr, 
                Utils::createSharedPtr(SCREEN_HEIGHT),
                Utils::VarType::INTEGER, "=");
}

/*
    From a declaration statement of an array, convert the size of the array to a valid integer, rename the variable
    to itself without the brackets, and return the size of the array. Throw an error if anything goes wrong.

    Examples:
        int test[43] -> parseArrayDecl(name="test[43]") -> returns 43 and sets 'name' to "test"
        double other[]
*/
int parseArrayDecl(std::string& name) 
{
    //get the value in between the brackets
    size_t bracketStart = name.find("[");    
    size_t bracketEnd = name.find_last_of("]");

    if(bracketStart >= bracketEnd)
        throwRunnerError("Unable to parse array size from '" + name + "'");

    std::string bracketVal = name.substr(bracketStart+1, bracketEnd-bracketStart-1);

    //parse string for integer using Utils
    int arrSize = *(int*)Utils::convertToVariable(bracketVal, Utils::VarType::INTEGER).ptr.get();

    //remove the brackets from the name
    name = name.substr(0, bracketStart);

    //return the size of the array
    return arrSize;
}

void throwRunnerError(std::string message) {
    throw std::runtime_error("Program Runner failed! : " + message);
}