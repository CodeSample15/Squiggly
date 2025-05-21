#include "screen.hpp"
#include "built-in.hpp"
#include "graphics.hpp"

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
    //extract object color and store in graphics color struct
    SGraphics::Color objColor;

    uint8_t c[3];
    obj.getColor(c);

    objColor.r = c[0];
    objColor.g = c[1];
    objColor.b = c[2];

    //extract object position
    SGraphics::pixel objLoc;

    objLoc.x = obj.getX();
    objLoc.y = obj.getY();

    switch(obj.shape) {
        case BuiltIn::ObjectShape::RECT:
            SGraphics::draw_rect(objLoc, obj.getWidth(), obj.getHeight(), obj.getRotation(), objColor, true);
            break;

        case BuiltIn::ObjectShape::ELLIPSE:
            SGraphics::draw_ellipse(objLoc, obj.getWidth(), obj.getHeight(), obj.getRotation(), objColor, true);
            break;

        case BuiltIn::ObjectShape::TRIANGLE:
            SGraphics::draw_triangle(objLoc, obj.getWidth(), obj.getHeight(), obj.getRotation(), objColor, true);
            break;
    }
}

Screen screen = Screen();