#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <exception>

#include "linter.hpp"
#include "tokenizer.hpp"
#include "runner.hpp"

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
        std::cout << "Could not find file \'" << argv[1] << "\' in current directory. Exiting..." << std::endl;
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
    try {
        Tokenizer::tokenize(fileLines);
    } catch(const std::exception& e) {
        std::cerr << "\n" << e.what() << std::endl;
        return 0;
    }

    //Execute script
    try {
        //Runner::execute();
    } catch(const std::exception& e) {
        std::cerr << "\n" << e.what() << std::endl;
    }

    return 0;
}