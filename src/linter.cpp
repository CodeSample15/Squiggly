#include <string>
#include <stdexcept>

#include "linter.hpp"
#include "built-in.hpp"

std::string getCloseForOpen(char c);
void throwLinterError(std::string msg);

/*
    Removes all whitespace from the program. Makes parsing the code easier and more reliable later on
*/
void Linter::preprocess(std::vector<std::string>& lines) 
{
    BuiltIn::Print("Preprocessing script...\t", false);

    //remove unecessary whitespace
    for(size_t i=0; i < lines.size(); i++) {
        char quoteChar = '\0';
        
        for(size_t j=0; j<lines[i].length(); j++) {
            //ignore strings
            if(quoteChar!='\0') {
                if(lines[i][j] == quoteChar)
                    quoteChar = '\0';
                continue;
            } else if(lines[i][j] == '"' || lines[i][j] == '\'') {
                quoteChar = lines[i][j];
                continue;
            }

            if(lines[i][j] == ' ' || lines[i][j] == '\t') {
                //check to see if there is whitespace, that it's not being used to separate two distinct strings
                bool rightGood = j+1 >= lines[i].length() || !std::isalpha(lines[i][j+1]);
                bool leftGood = j==0 || !std::isalpha(lines[i][j-1]);

                if(lines[i][j] == '\t' || rightGood || leftGood) {
                    lines[i].erase(j, 1);
                    j--;
                }
            }
            else if(lines[i][j] == COMMENT_PREFIX) { //remove comments
                lines[i].erase(j, lines[i].length());
                break;
            }
        }
    }

    //delete empty lines
    for(size_t i=0; i < lines.size(); i++) {
        if(lines[i].length() == 0) {
            lines.erase(lines.begin() + i);
            i--;
        }
    }

    BuiltIn::Print("Done");
}

/*
    "lint" is my fancy way of saying "make sure the programmer used proper formatting"
    This means:
        - all open braces have matching close braces
        - all strings are opened and closed on the same line
        - etc
*/
void Linter::lint(std::vector<std::string>& lines) 
{
    BuiltIn::Print("Checking script...\t", false);
    std::vector<char> stack; //throwback to freshmen datastructures and algorithms: make sure each open bracket/brace has a close bracket/brace

    for(size_t i=0; i<lines.size(); i++) {
        bool skippingString = false;
        for(char c : lines[i]) {
            if(skippingString && c != '"')
                continue;

            switch(c) {
                case '{':
                    stack.push_back(c);
                    break;

                case '}':
                    if(!stack.empty() && stack.back() == '{')
                        stack.pop_back();
                    else
                        throwLinterError("Error at line " + std::to_string(i+1) + ": '}' has no open brace!");
                    break;

                case '[':
                    stack.push_back(c);
                    break;

                case ']':
                    if(!stack.empty() && stack.back() == '{')
                        stack.pop_back();
                    else
                        throwLinterError("Error at line " + std::to_string(i+1) + ": ']' has no open bracket!");
                    break;

                case '"':
                    if(skippingString)
                        stack.pop_back();
                    else
                        stack.push_back(c);
                    skippingString = !skippingString;
                    break;

                default:
                    continue;
            }
        }

        //check stack at the end of each line, make sure everything is closed on the same line except for braces
        if(!stack.empty() && stack.back() != '{')
                throwLinterError("Error at line " + std::to_string(i+1) + ": Expected closing '" + getCloseForOpen(stack.back()) + "' in line");
    }

    if(!stack.empty())
        throwLinterError("Not all braces are closed! Make sure each '{' has a matching '}'");

    BuiltIn::Print("Done");
}

std::string getCloseForOpen(char c) {
    switch(c) {
        case '{':
            return "}";
        case '[':
            return "]";
        case '"':
            return "\"";
        default:
            return "";
    }
}

void throwLinterError(std::string msg) {
    throw std::runtime_error("Linter Failed! : " + msg);
}