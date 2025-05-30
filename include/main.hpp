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
        //no filename provided, print instructions
        std::cout << "------------------------------" << std::endl;
        std::cout << "SQUIGGLY PROGRAMMING LANGUAGE:" << std::endl;
        std::cout << "------------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << std::endl;
        std::cout << " - Run program: squiggly <filename>" << std::endl;
        std::cout << " - Print template: squiggly template" << std::endl;
        std::cout << std::endl;
        std::cout << "Stuck? Check out some example scripts: " << std::endl;
        std::cout << "https://github.com/CodeSample15/Squiggly/tree/main/test_scripts" << std::endl;
        return 2;
    } else if(strcmp(argv[1], "template")==0) {
        //print out template project so that it can be copy-pasted into a file
        std::cout << "# PROJECT TEMPLATE: copy-paste this into a file to get started on a project" << std::endl;
        std::cout << std::endl;
        std::cout << ":VARS: {\n    # global variables go here\n}\n\n:START: {\n    # initialization code goes here\n}\n\n:UPDATE: {\n    # game loop logic goes here\n}\n" << std::endl;
        return 2;
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
        // if(temp.empty())
        //     continue; //skip empty lines

        fileLines.push_back(temp);
    }

    file.close();
    return 0;
}

int run_squiggly(std::vector<std::string>& fileLines) {
    try {
        Linter::lint(fileLines);
    } catch(const std::exception& e) {
        std::cerr << "\n" << e.what() << std::endl;
        return 0;
    }

    //preprocess raw strings
    Linter::preprocess(fileLines);

    //Run tokenizer
    try {
        Tokenizer::tokenize(fileLines);
    } catch(const std::exception& e) {
        std::cerr << "\n" << e.what() << std::endl;
        return 0;
    }

    //Execute script (stored in tokenizer)
    try {
        Runner::execute();
    } catch(const std::exception& e) {
        std::cerr << "\n" << e.what() << std::endl;
    }

    std::cout << "Program stopped successfully" << std::endl;

    return 0;
}