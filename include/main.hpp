#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "tokenizer.hpp"
#include "runner.hpp"
#include "linter.hpp"

int parse_args(int argc, char** argv, std::vector<std::string>& fileLines);
int read_file(char* path, std::vector<std::string>& fileLines);
int run_squiggly(std::vector<std::string>& fileLines);


/*
    Main Squiggly functionality:
*/
int parse_args(int argc, char** argv, std::vector<std::string>& fileLines) {
    //check to make sure file was passed as input to the program
    if(argc < 2) {
        std::cout << "No file not provided. Exiting..." << std::endl;
        return 0;
    }

    return read_file(argv[1], fileLines);
}

int read_file(char* path, std::vector<std::string>& fileLines) {
    //attempt to open and then read from file
    std::ifstream file;
    file.open(path);
    if(!file.is_open()) {
        std::cout << "Could not find file \'" << path << "\' in current directory. Exiting..." << std::endl;
        return 1;
    }

    //read from the input file
    std::string temp;
    while(std::getline(file, temp)) {
        if(temp.empty())
            continue; //skip empty lines

        fileLines.push_back(temp);
    }

    file.close();
    return 0;
}

int run_squiggly(std::vector<std::string>& fileLines) {
    // try {
    // Linter::lint(fileLines);
    // }


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
        Runner::execute();
    } catch(const std::exception& e) {
        std::cerr << "\n" << e.what() << std::endl;
    }

    std::cout << "Program stopped successfully" << std::endl;

    return 0;
}