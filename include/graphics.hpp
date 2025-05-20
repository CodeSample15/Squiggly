#pragma once

#include "screen.hpp"

namespace SGraphics {
    typedef struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } Color;

    void draw_rect(int x, int y, int width, int height, float rot, Color color, bool fill=false);
    void draw_triangle(int x, int y, int width, int height, float rot, Color color, bool fill=false);
    void draw_ellipse(int x, int y, int width, int height, float rot, Color color, bool fill=false);
    void draw_polygon(int x1, int y1, int x2, int y2, int x3, int y3, Color color, bool fill=false);
    void draw_line(int x1, int y1, int x2, int y2, Color color);
}