#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cctype>

//special unique patterns that will be searched for in the code (makes the language more customizable if needed)
#define VAR_FUNC_HEAD ":VARS:"
#define START_FUNC_HEAD ":START:"
#define UPDATE_FUNC_HEAD ":UPDATE:"

namespace Linter {
    void preprocess(std::vector<std::string>& lines);
}