#pragma once

#include "utils.hpp"

#define TOKENIZED_PTR std::shared_ptr<Tokenizer::TokenizedLine> 

namespace Runner {
    //run each segment of the code once
    void executeVars();
    void executeStart();
    void executeUpdate();

    //run the whole program and loop executeUpdate
    void execute();

    //allow external cpp files to access variables
    Utils::SVariable* fetchVariable(std::string varName);
}