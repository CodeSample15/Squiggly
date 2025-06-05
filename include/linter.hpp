#pragma once

#include <vector>
#include <string>

//Special unique patterns that will be searched for in the code (makes the language more customizable if needed)
#define VAR_FUNC_HEAD ":VARS:"
#define START_FUNC_HEAD ":START:"
#define UPDATE_FUNC_HEAD ":UPDATE:"

#define LOOP_HEADER "repeat"
#define WHILE_LOOP_HEADER "while"

//Special characters to look for
#define COMMENT_PREFIX '#'
#define BUILT_IN_VAR_PREFIX '$'
#define IMAGE_DECLARATION_PREFIX '@'
#define STRING_CONCAT_CHAR '+'

namespace Linter {
    void preprocess(std::vector<std::string>& lines);
    void lint(std::vector<std::string>& lines);
}