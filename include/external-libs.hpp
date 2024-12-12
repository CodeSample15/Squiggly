/*
    Compile external libraries in seperate file to avoid long compile times
    for the main project during development.

    This header file will provide handles for other files to access the
    functions from the external libraries that are needed for this project.
*/

#include <string>

double expressionToDouble(std::string exp);