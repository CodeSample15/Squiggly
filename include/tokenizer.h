#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace Tokenizer {
    typedef enum Type {
        CALL,
        BRANCH,
        ASSIGN,
        NAME
    } LineType;

    typedef struct Line {
        LineType type;

        int callLineNum;
        std::string params;

        int branchLineNumTRUE;
        int branchLineNumFALSE;

        std::string leftAssign;
        std::string rightAssign;

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