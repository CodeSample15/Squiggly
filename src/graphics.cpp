/*
    Simple graphics library to draw shapes to a virtual screen.

    Each function in the SGraphics namespace will draw to the screen buffer in screen.hpp: uint8_t screenBuff[SCREEN_HEIGHT][SCREEN_WIDTH][3]
*/

#include "graphics.hpp"

/*
    Simple helper method to draw a pixel on the screen without causing an OOB error
 */
void draw_pixel(SGraphics::pixel p, SGraphics::Color c) 
{
    if(p.x>=0 && p.x<SCREEN_WIDTH && p.y>=0 && p.y<SCREEN_HEIGHT) {
        screen.screenBuff[p.x][p.y][0] = c.r;
        screen.screenBuff[p.x][p.y][1] = c.g;
        screen.screenBuff[p.x][p.y][2] = c.b;
    }
}

/**
 * @brief Draws a rectangle to screenBuff
 * 
 * Center of the rectangle is at location loc. Rectangle can be at any rotation rot in degrees. 
 * 
 * @param loc
 * @param width 
 * @param height 
 * @param rot 
 * @param color 
 * @param fill 
 */
void SGraphics::draw_rect(pixel loc, int width, int height, float rot, Color color, bool fill) 
{
    
}

/**
 * @brief Draw a simple triangle to screenBuff. 
 * 
 * Center of triangle is located at location loc. 
 * Triangle is an equilateral triangle that can be stretched using width and height.
 * 
 * @param loc
 * @param width 
 * @param height 
 * @param rot 
 * @param color 
 * @param fill 
 */
void SGraphics::draw_triangle(pixel loc, int width, int height, float rot, Color color, bool fill) 
{

}

/**
 * @brief Draw a circle to screenBuff
 * 
 * Center is located at location loc. Circles are ellipses that have width==height.
 * 
 * @param loc
 * @param width 
 * @param height 
 * @param rot 
 * @param color 
 * @param fill 
 */
void SGraphics::draw_ellipse(pixel loc, int width, int height, float rot, Color color, bool fill) 
{

}

/**
 * @brief Draw a polygon (triangle) between three different points on the screen
 * 
 * @param one
 * @param two
 * @param three
 * @param color 
 * @param fill 
 */
void SGraphics::draw_polygon(pixel one, pixel two, pixel three, Color color, bool fill) 
{
    if(fill) {
        //  Bounding box for triangle
        int startX = std::min({one.x, two.x, three.x});
        int endX = std::max({one.x, two.x, three.x});

        int startY = std::min({one.y, two.y, three.y});
        int endY = std::max({one.y, two.y, three.y});

        // Compute the area of the triangle (using the determinant)
        float denom = (two.y - three.y) * (one.x - three.x) + (three.x - two.x) * (one.y - three.y);

        //  No divide by 0 or very small triangles
        if (std::abs(denom) < 1e-5f)
            return;

        //temp variable for holding coordinates
        pixel p;

        //  Barycentric to determine if point is in triangle
        for(int x = startX; x <= endX; x++)
        {
            for(int y = startY; y <= endY; y++)
            {
                float w0, w1, w2;

                // Compute the barycentric weights
                w0 = ((two.y - three.y) * (x - three.x) + (three.x - two.x) * (y - three.y)) / denom;
                w1 = ((three.y - one.y) * (x - three.x) + (one.x - three.x) * (y - three.y)) / denom;
                w2 = 1.0f - w0 - w1;

                //  If weights are all valid, inside triangle
                if(w0 >= 0 && w1 >= 0 && w2 >= 0)
                {
                    p.x = x;
                    p.y = y;
                    draw_pixel(p, color);
                }
            }
        }
    }
    else {
        //we are not filling the triangle, just draw lines
        draw_line(one, two, color);
        draw_line(one, three, color);
        draw_line(three, two, color);
    }
}

/**
 * @brief Draw a line between two points on the screen
 * 
 * @param one
 * @param two
 * @param color 
 */
void SGraphics::draw_line(pixel one, pixel two, Color color) 
{
    //  Slopes calculation
    int x0 = one.x, x1 = two.x;
    int y0 = one.y, y1 = two.y;

    int dx = abs(two.x - one.x);
    int dy = abs(two.y - one.y);

    int signx = (one.x < two.x) ? 1 : -1;
    int signy = (one.y < two.y) ? 1 : -1;

    int err = dx - dy;

    //  First pixel
    SGraphics::pixel p;
    p.x = x0;
    p.y = y0;
    draw_pixel(p, color);

    //  Draw Pixels
    while(true)
    {
        // End condition
        if (x0 == x1 && y0 == y1)
            break;

        int err2 = 2 * err;

        if (err2 > -dy)
        {
            err -= dy;
            x0 += signx;
        }

        if (err2 < dx)
        {
            err += dx;
            y0 += signy;
        }

        //  Draw to screen buffer
        p.x = x0;
        p.y = y0;
        draw_pixel(p, color);
    }
}