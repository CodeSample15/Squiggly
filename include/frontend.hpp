/*
    This file contains handles for each display function needed to make Squiggly work

    The aim of this file is to loosly couple the frontend side of Squiggly so that, when porting
    the language to dedicated raspberry pi console, the function names used by other parts of the code can remain the
    same while the actual implementation can be treated differently.
*/

#pragma once

//this will allow both the raspi port and the pc port to be in the same codebase
#define BUILD_FOR_RASPI false

#if BUILD_FOR_RASPI
#include "ST7735_TFT.hpp"
#endif

namespace Frontend {
    //input control: -------------------------------

    #if BUILD_FOR_RASPI
    void updateReadings();
    ST7735_TFT* get_tft();
    #endif

    //directional control
    float getHorAxis();
    float getVertAxis();

    //A/B buttons for additional actions
    bool getABtn();
    bool getBBtn();

    //for exiting programs
    bool getExitBtn();

    //output display: ------------------------------

    void drawScreen();
    void init();
    void cleanUp();
}