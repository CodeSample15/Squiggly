#pragma once

#include <vector>
#include <string>

#include "frontend.hpp"

#define TEXT_PIXEL_HEIGHT 15
#define TEXT_LEFT_BUFFER 5
#define SCRIPT_PATH std::string("../test_scripts")


/*
    This entire file will be written with the assumption that BUILD_FOR_RASPI is true
    This file SHOULD NOT be included anywhere unless it is made certain that the BUILD_FOR_RASPI flag is true (defined in frontend.hpp)
*/

/*
    Draws the selection menu on the TFT screen
*/
std::string run_menu();