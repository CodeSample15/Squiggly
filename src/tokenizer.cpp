#include "tokenizer.h"

using namespace Tokenizer;

std::vector<TokenizedLine> varsBlock_tok = std::vector<TokenizedLine>();
std::vector<TokenizedLine> startBlock_tok = std::vector<TokenizedLine>();
std::vector<TokenizedLine> mainLoop_tok = std::vector<TokenizedLine>();

std::vector<std::vector<TokenizedLine>> functions_tok = std::vector<std::vector<TokenizedLine>>();

void clearTokenized(); //reset all of the tokenized objects to be empty (clear memory of old program if it exists)

void Tokenizer::tokenize(std::vector<std::string> lines) 
{
    clearTokenized();
}

void clearTokenized() {
    varsBlock_tok.clear();
    startBlock_tok.clear();
    mainLoop_tok.clear();

    for(size_t i=0; i<functions_tok.size(); i++) {
        functions_tok[i].clear();
    }
    functions_tok.clear();
}