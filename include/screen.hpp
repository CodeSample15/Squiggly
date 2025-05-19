#pragma once

#include <cstdint>
#include <vector>

#include "built-in.hpp"

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

//Dimensions for console
//#define SCREEN_WIDTH 160
//#define SCREEN_HEIGHT 128

#define SCREEN_REFRESH_DELAY 20

typedef struct {
    uint16_t x;
    uint16_t y;
} screen_loc;

class Screen {
    public:
        Screen();

        void clear();
        void drawObj(BuiltIn::Object& obj);

        uint8_t screenBuff[SCREEN_HEIGHT][SCREEN_WIDTH][3];
};

extern Screen screen;