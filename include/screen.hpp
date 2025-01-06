#pragma once

#include <cstdint>

#include "built-in.hpp"

#define SCREEN_WIDTH 200
#define SCREEN_HEIGHT 100

#define SCREEN_REFRESH_DELAY 20

class Screen {
    public:
        Screen();

        void clear();
        void drawObj(BuiltIn::Object& obj);

        uint8_t screenBuff[SCREEN_HEIGHT][SCREEN_WIDTH][3];
};

extern Screen screen;