/*
    This file contains handles for each display function needed to make Squiggly work

    The aim of this file is to loosly couple the frontend side of Squiggly so that, when porting
    the language to arduino, the function names used by other parts of the code can remain the
    same while the actual implementation can be treated differently.

    Basically, everything that interfaces with Squiggly that is external to the language itself will go through
    this header file.

    For the PC version of Squiggly, games will be rendered to an opencv window and game input will
    be taken from the keyboard.
    For the arduino version of Squiggly, games will be rendered to a physical screen wired to the arduino
    and game input will be taken from physical buttons also wired to the arduino.
*/

#pragma once

//this will allow both the arduino port and the pc port to be in the same codebase
#define BUILD_FOR_RASPI true

#if BUILD_FOR_RASPI
#include "ST7735_TFT.hpp"
#endif

//for windows builds
#define WIN_A_BTN_CODE 0x5A
#define WIN_B_BTN_CODE 0x58

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