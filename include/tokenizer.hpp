#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Tokenizer {
    enum LineType {
        CALL,           //calling a function
        BI_CALL,        //call to 'built-in' function/variable
        BRANCH,         //if/else statements
        BRANCH_ELSE,    //else statements (uses if/else values)
        LOOP,           //loops
        ASSIGN,         //assign a variable to a value
        DECLARE,        //declare a variable
        DECLARE_ASSIGN, //declare a variable and assign a value in one line
        FUNC_NAME       //name of a function / header of some code
    };

    //base tokenized line class
    class TokenizedLine {
        public:
            LineType type;
            size_t srcLine; // line of code in the source file so programmers have better error tracing (NOT IMPLEMENTED YET)
    };

    //tokenized line varients
    class CallLine : public TokenizedLine {
        public:
            std::string callFuncName;
            std::vector<std::string> args;
    };

    class BranchLine : public TokenizedLine {
        public:
            std::string booleanExpression;
            size_t branchLineNumTRUE;
            size_t branchLineNumELSE;
            bool ifElse;
            size_t id; //keep track of different branch statements lined up next to each other
    };

    class LoopLine : public TokenizedLine {
        public:
            std::string loopTimes;
            size_t loopStart;
            size_t loopEnd;
            bool isWhile;
    };

    class AssignLine : public TokenizedLine {
        public: //linetype can be either ASSIGN or DECLARE_ASSIGN
            std::string assignDst;
            std::string assignSrc;
            std::string assignType; //DECLARE ASSIGN
            std::string assignOperator; //=, -=, +=, etc
    };

    class DeclareLine : public TokenizedLine {
        //TODO: allow programmers to use auto-type assignment
        public:
            std::string varName;
            std::string varType;
    };

    class FuncNameLine : public TokenizedLine {
        public:
            std::string funcName;
            std::vector<std::string> expectedArgs;
    };

    //method that other scripts will be using
    void tokenize(std::vector<std::string>& lines);
}

//pointers for different methods in the program (keeps code separate and easier to manage in memory imo)
extern std::vector< std::shared_ptr<Tokenizer::TokenizedLine> > varsBlock_tok;
extern std::vector< std::shared_ptr<Tokenizer::TokenizedLine> > startBlock_tok;
extern std::vector< std::shared_ptr<Tokenizer::TokenizedLine> > mainLoop_tok;
extern std::vector<std::vector< std::shared_ptr<Tokenizer::TokenizedLine> >> functions_tok;