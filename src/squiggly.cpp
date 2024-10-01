#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "tokenizer.h"
#include "linter.h"

int main(int argc, char** argv) {
    //quick check to make sure file was passed as input to the program
    if(argc < 2) {
        std::cout << ".sqgly file not provided. Exiting..." << std::endl;
        return 0;
    }

    std::vector<std::string> fileLines;

    //attempt to open and then read from file
    std::ifstream file;
    file.open(argv[1]);
    if(!file.is_open()) {
        std::cout << "Could not find file \'" << argv[0] << "\' in current directory. Exiting..." << std::endl;
        return 0;
    }

    //read from the input file
    std::string temp;
    while(std::getline(file, temp)) {
        if(temp.empty())
            continue; //skip empty lines

        fileLines.push_back(temp);
    }
    file.close();

    //TODO: Run linter
    Linter::preprocess(fileLines);

    //Run tokenizer
    //Tokenizer::tokenize(fileLines);

    //TODO: Run script

    return 0;
}