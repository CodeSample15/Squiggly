#include "linter.hpp"
#include "built-in-funcs.hpp"

/*
    Removes all whitespace from the program. Makes parsing the code easier and more reliable later on
*/
void Linter::preprocess(std::vector<std::string>& lines) 
{
    BuiltIn::Print("Preprocessing script...\t", false);

    //remove unecessary whitespace
    for(size_t i=0; i < lines.size(); i++) {
        for(size_t j=0; j<lines[i].length(); j++) {
            if(lines[i][j] == ' ' || lines[i][j] == '\t') {
                //check to see if there is whitespace, that it's not being used to separate two distinct strings
               bool rightGood = j+1 >= lines[i].length() || !std::isalpha(lines[i][j+1]);
               bool leftGood = j-1 >= 0 && !std::isalpha(lines[i][j-1]);

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