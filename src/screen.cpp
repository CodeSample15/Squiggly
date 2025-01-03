#include "screen.hpp"
#include "built-in.hpp"

Screen::Screen() {
    clear();
}

void Screen::clear() {
    for(uint8_t x=0; x<SCREEN_WIDTH; x++) {
        for(uint8_t y=0; y<SCREEN_HEIGHT; y++) {
            screenBuff[x][y][0] = 0; //r
            screenBuff[x][y][1] = 0; //g
            screenBuff[x][y][2] = 0; //b
        }
    }
}

void Screen::drawObj(BuiltIn::Object& obj)
{
    
}

Screen screen = Screen();