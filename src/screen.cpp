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
        for(int x=0; x<obj.getWidth(); x++) {
            int xDraw = x+(int)obj.getX();
            int yDraw = y+(int)obj.getY();
            if(xDraw < 0 || xDraw >= SCREEN_WIDTH || yDraw < 0 || yDraw >= SCREEN_HEIGHT)
                continue;

            uint8_t objColor[3];
            obj.getColor(objColor);
            screenBuff[yDraw][xDraw][0] = objColor[0];
            screenBuff[yDraw][xDraw][1] = objColor[1];
            screenBuff[yDraw][xDraw][2] = objColor[2];
        }
    }
}

std::vector<screen_loc> Screen::changedPixels(Screen& other) 
{
    std::vector<screen_loc> changed;
    screen_loc temp;
    for(int y=0; y<SCREEN_WIDTH; y++) {
        for(int x=0; x<SCREEN_HEIGHT; x++) {
            if(other.screenBuff[y][x][0] != screenBuff[y][x][0] || other.screenBuff[y][x][1] != screenBuff[y][x][1] || other.screenBuff[y][x][2] != screenBuff[y][x][2]) 
            {
                temp.x = y;
                temp.y = x;
                changed.push_back(temp);
            }
        }
    }

    return changed;
}

Screen screen = Screen();