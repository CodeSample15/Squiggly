/*
    Simple graphics library to draw shapes to a virtual screen.

    Each function in the SGraphics namespace will draw to the screen buffer in screen.hpp: uint8_t screenBuff[SCREEN_HEIGHT][SCREEN_WIDTH][3]
*/

#include "graphics.hpp"

/**
 * @brief Draws a rectangle to screenBuff
 * 
 * Center of the rectangle is at x, y. Rectangle can be at any rotation rot in degrees. 
 * 
 * @param x 
 * @param y 
 * @param width 
 * @param height 
 * @param rot 
 * @param color 
 * @param fill 
 */
void SGraphics::draw_rect(int x, int y, int width, int height, float rot, Color color, bool fill) 
{

}

/**
 * @brief Draw a simple triangle to screenBuff. 
 * 
 * Center of triangle is located at x, y. 
 * Triangle is an equilateral triangle that can be stretched using width and height.
 * 
 * @param x 
 * @param y 
 * @param width 
 * @param height 
 * @param rot 
 * @param color 
 * @param fill 
 */
void SGraphics::draw_triangle(int x, int y, int width, int height, float rot, Color color, bool fill) 
{

}

/**
 * @brief Draw a circle to screenBuff
 * 
 * Center is located at x, y. Circles are ellipses that have width==height.
 * 
 * @param x 
 * @param y 
 * @param width 
 * @param height 
 * @param rot 
 * @param color 
 * @param fill 
 */
void SGraphics::draw_ellipse(int x, int y, int width, int height, float rot, Color color, bool fill) 
{

}

/**
 * @brief Draw a polygon (triangle) between three different points on the screen
 * 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param x3 
 * @param y3 
 * @param color 
 * @param fill 
 */
void SGraphics::draw_polygon(int x1, int y1, int x2, int y2, int x3, int y3, Color color, bool fill) 
{

}

/**
 * @brief Draw a line between two points on the screen
 * 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param color 
 */
void SGraphics::draw_line(int x1, int y1, int x2, int y2, Color color) 
{

}