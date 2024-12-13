#include <vector>
#include <memory>
#include <stdexcept>

#include "runner.hpp"
#include "tokenizer.hpp"
#include "built-in-funcs.hpp"
#include "utils.hpp"

using namespace Runner;

typedef std::shared_ptr<Tokenizer::TokenizedLine> TOKENIZED_PTR;

//virtual memory buffers
std::vector<Utils::SVariable> gVars;    //global variables
std::vector<Utils::SVariable> sVars;    //stack variables

//useful functions
void runProgram(std::vector<TOKENIZED_PTR>& tokens, std::vector<Utils::SVariable>& memory, size_t stackFrameIdx, bool clearStackWhenDone=true, size_t startIdx=0, size_t endIdx=0); //general function for running blocks of code
void runUserFunction(std::string name, std::vector<std::string>& args); //find a user defined function and run
void setVariable(const std::shared_ptr<void>& dst, const std::shared_ptr<void>& src, Utils::VarType type, std::string assignType="="); //assign one value to another value
inline void throwError(std::string message); //throw a runner error

//useful for debugging to have these separated
void Runner::executeVars() { runProgram(varsBlock_tok, gVars, 0, false); }
void Runner::executeStart() { runProgram(startBlock_tok, sVars, 0); }
void Runner::executeUpdate() { runProgram(mainLoop_tok, sVars, 0); }

void Runner::flushMem() {
    gVars.clear();
    sVars.clear();
}

//main execute function with loop
void Runner::execute() 
{
    flushMem();

    executeVars();
    executeStart();

    //TODO: put in escapable while loop
    executeUpdate();
}

Utils::SVariable* Runner::fetchVariable(std::string name) 
{
    //search local stack frame first (start at most recently pushed variable and work backwards, hopefully should add some efficiency)
    for(ssize_t i=sVars.size()-1; i>=0; i--) {
        if(sVars[i].name == name)
            return &sVars[i];
    }

    //search global variables
    for(Utils::SVariable& var : gVars) {
        if(var.name == name)
            return &var;
    }

    return nullptr; //no variable was found, return a nullptr as a safety guard
}

/*
    Execute a block of the program.

    Starts from a particular location inside the given block of tokens, and ends at either the end of the token block if endIdx is 0, or at 
    position endIdx if the value is not 0
*/
void runProgram(std::vector<TOKENIZED_PTR>& tokens, std::vector<Utils::SVariable>& memory, size_t stackFrameIdx, bool clearStackWhenDone, size_t startIdx, size_t endIdx) 
{
    endIdx = endIdx==0 ? tokens.size() : endIdx;

    Tokenizer::CallLine* callLine;
    //Tokenizer::BranchLine* branchLine;
    //Tokenizer::LoopLine* loopLine;
    Tokenizer::AssignLine* assignLine;
    Tokenizer::DeclareLine* declareLine;
    //Tokenizer::FuncNameLine* funcNameLine;

    Utils::SVariable* varBuff;
    Utils::SVariable newVariableHolder;
    
    //iterate through the program
    for(size_t prgCounter = startIdx; prgCounter < endIdx; prgCounter++) {
        TOKENIZED_PTR line = tokens[prgCounter];

        //execute instruction depending on what type of line is next in the buffer
        switch(line->type) {
            case Tokenizer::LineType::CALL:
                callLine = (Tokenizer::CallLine*)line.get();
                runUserFunction(callLine->callFuncName, callLine->args);
                break;

            case Tokenizer::LineType::BI_CALL:
                callLine = (Tokenizer::CallLine*)line.get();
                BuiltIn::runFunction(callLine->callFuncName, callLine->args);
                break;

            case Tokenizer::LineType::ASSIGN:
                assignLine = (Tokenizer::AssignLine*)line.get();
                varBuff = fetchVariable(assignLine->assignDst);
                if(varBuff)
                    setVariable(varBuff->ptr, Utils::convertToVariable(assignLine->assignSrc, varBuff->type).ptr, varBuff->type, assignLine->assignOperator);
                else
                    throwError("Unable to find variable " + assignLine->assignDst);
                break;

            case Tokenizer::LineType::DECLARE:
                declareLine = (Tokenizer::DeclareLine*)line.get();
                newVariableHolder.name = declareLine->varName;
                newVariableHolder.type = Utils::stringToVarType(declareLine->varType);
                newVariableHolder.ptr = Utils::createEmptyShared(newVariableHolder.type);
                memory.push_back(newVariableHolder); //push new variable to stack
                break;

            case Tokenizer::LineType::DECLARE_ASSIGN:
                assignLine = (Tokenizer::AssignLine*)line.get();
                newVariableHolder.name = assignLine->assignDst;
                newVariableHolder.type = Utils::stringToVarType(assignLine->assignType);
                newVariableHolder.ptr = Utils::createEmptyShared(newVariableHolder.type);

                setVariable( newVariableHolder.ptr, 
                        Utils::convertToVariable(assignLine->assignSrc, newVariableHolder.type).ptr, 
                        newVariableHolder.type, 
                        assignLine->assignOperator );
                
                memory.push_back(newVariableHolder); //push new variable to stack
                break;

            default:
                BuiltIn::Print("Unknown line!");
                break;
        }
    }

    //clear out stack frame
    if(clearStackWhenDone) {
        size_t numVars = memory.size() - stackFrameIdx; //number of variables created in this stack frame
        for(size_t i=0; i<numVars; i++)
            memory.erase(memory.begin() + stackFrameIdx);
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
                    throwError("Unexpected number of arguments passed to function " + name);

                //get current stack size so stack frame can be cleared later
                size_t functionStackFrame = sVars.size();

                //add arguments to stack
                for(size_t i=0; i<args.size(); i++) {
                    //get variable's new name from the function parameter vector
                    std::string expected = tmp->expectedArgs[i];
                    size_t spaceLocation = expected.find(" ");
                    if(spaceLocation == std::string::npos)
                        throwError("Improper parameter declaration of function " + name); //quick error check because I don't trust the Linter

                    //extract the expected name and type
                    std::string etype = expected.substr(0, spaceLocation);
                    std::string ename = expected.substr(spaceLocation+1, expected.length()-spaceLocation-1);

                    Utils::SVariable nextVar = Utils::convertToVariable(args[i], Utils::stringToVarType(etype));
                    nextVar.name = ename; //split expected arg into type and name, get the name
                    sVars.push_back(nextVar); //push to stack
                }

                runProgram(function, sVars, functionStackFrame, true, 1);
                break;
            }
        } else {
            throwError("Tokenizer screwed up function tokenizing!"); //idealy, the programmer should NEVER get this error, I'm just putting this here to call out Squiggly development screw ups
        }
    }

    if(!found)
        throwError("Cannot find function named " + name);
}

/*
    Assigns a value to a variable using a specific assignType if necessary
    Casts the void pointers passed into the expected datatype pointers and dereferences
*/
void setVariable(const std::shared_ptr<void>& dst, const std::shared_ptr<void>& src, Utils::VarType type, std::string assignType) {
    switch(type) {
        case Utils::VarType::STRING:
            if(assignType=="=")
                *((std::string*)dst.get()) = *((std::string*)src.get());
            else if(assignType=="+=")
                *((std::string*)dst.get()) += *((std::string*)src.get());
            else
                throwError("Invalid assign operator '" + assignType + "' for string type");
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
                throwError("Invalid assign operator '" + assignType + "' for string type");
            break;

        case Utils::VarType::DOUBLE:
            if(assignType=="=")
                *((double*)dst.get()) = *((double*)src.get());
            else if(assignType=="+")
                *((double*)dst.get()) += *((double*)src.get());
            else if(assignType=="-=")
                *((double*)dst.get()) -= *((double*)src.get());
            else if(assignType=="*=")
                *((double*)dst.get()) *= *((double*)src.get());
            else if(assignType=="/=")
                *((double*)dst.get()) /= *((double*)src.get());
            else
                throwError("Invalid assign operator '" + assignType + "' for string type");
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
                throwError("Invalid assign operator '" + assignType + "' for string type");
            break;

        case Utils::VarType::BOOL:
            if(assignType=="=")
                *((float*)dst.get()) = *((float*)src.get());
            else
                throwError("Invalid assign operator '" + assignType + "' for string type");
            break;

        default:
            throwError("Error setting variable (This is a problem with Squiggly, not with your Squiggly code. Please report this issue on the project's GitHub)");
            break;
    }
}

inline void throwError(std::string message) {
    throw std::runtime_error("Program Runner failed! : " + message);
}