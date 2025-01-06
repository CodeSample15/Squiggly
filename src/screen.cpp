#include "screen.hpp"
#include "built-in.hpp"

Screen::Screen() {
    clear();
}

void Screen::clear() {
    for(size_t x=0; x<SCREEN_WIDTH; x++) {
        for(size_t y=0; y<SCREEN_HEIGHT; y++) {
            screenBuff[y][x][0] = 0; //r
            screenBuff[y][x][1] = 0; //g
            screenBuff[y][x][2] = 0; //b
        }
    }
}

void Screen::drawObj(BuiltIn::Object& obj)
{
    for(int y=0; y<obj.getHeight(); y++) {
        for(int x=0; x<obj.getHeight(); x++) {
            int xDraw = x+(int)obj.getX();
            int yDraw = y+(int)obj.getY();
            if(xDraw < 0 || xDraw >= SCREEN_WIDTH || yDraw < 0 || yDraw >= SCREEN_HEIGHT)
                continue;

            //load colors in BGR order
            screenBuff[yDraw][xDraw][0] = obj.color[2];
            screenBuff[yDraw][xDraw][1] = obj.color[1];
            screenBuff[yDraw][xDraw][2] = obj.color[0];
        }
    }
}

Screen screen = Screen();