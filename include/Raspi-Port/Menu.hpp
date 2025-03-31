#pragma once

#include <vector>
#include <string>

#include "frontend.hpp"

/*
    This entire file will be written with the assumption that BUILD_FOR_RASPI is true
    This file SHOULD NOT be included anywhere unless it is made certain that the BUILD_FOR_RASPI flag is true (defined in frontend.hpp)
*/

/*
    Draws the selection menu on the 
*/
void run_menu(std::vector<std::string>& fileLines);