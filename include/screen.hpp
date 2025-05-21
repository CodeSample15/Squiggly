#pragma once

#include <cstdint>
#include <vector>

#include "built-in.hpp"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 500

//Dimensions for console
//#define SCREEN_WIDTH 160
//#define SCREEN_HEIGHT 128

class Screen {
    public:
        Screen();

        void clear();
        void drawObj(BuiltIn::Object& obj);

        uint8_t screenBuff[SCREEN_HEIGHT][SCREEN_WIDTH][3];
};

extern Screen screen;