#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace Tokenizer {
    enum LineType {
        CALL,           //calling a function
        BRANCH,         //if/else statements
        ASSIGN,         //assign a variable to a value
        DECLARE,        //declare a variable
        DECLARE_ASSIGN, //declare a variable and assign a value in one line
        FUNC_NAME       //name of a function / header of some code
    };

    typedef struct Line {
        LineType type;
        int srcLine; // line of code in the source file so programmers have better error tracing

        //CALL
        int callLineNum;
        std::string params;

        //BRANCH
        int branchLineNumTRUE;
        int branchLineNumFALSE;

        //ASSIGN
        std::string assignDst;
        std::string assignSrc;
        std::string assignType; //DECLARE ASSIGN

        std::string funcName;
    } TokenizedLine;

    void lint(std::vector<std::string> lines); //TODO: write linter to check code before being sent over to tokenizer
    void tokenize(std::vector<std::string>& lines);
}

//pointers for different methods in the program (keeps code separate and easier to manage in memory imo)
extern std::vector<Tokenizer::TokenizedLine> varsBlock_tok;
extern std::vector<Tokenizer::TokenizedLine> startBlock_tok;
extern std::vector<Tokenizer::TokenizedLine> mainLoop_tok;
extern std::vector<std::vector<Tokenizer::TokenizedLine>> functions_tok;