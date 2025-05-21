#pragma once

#include "screen.hpp"

namespace SGraphics {
    typedef struct {
        uint16_t x;
        uint16_t y;
    } pixel;

    typedef struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } Color;

    void draw_rect(pixel loc, int width, int height, float rot, Color color, bool fill=false);
    void draw_triangle(pixel loc, int width, int height, float rot, Color color, bool fill=false);
    void draw_ellipse(pixel loc, int width, int height, float rot, Color color, bool fill=false);
    void draw_polygon(pixel one, pixel two, pixel three, Color color, bool fill=false);
    void draw_line(pixel one, pixel two, Color color);
}