#include "physics.hpp"

using namespace Physics;

void rotate_point(int& x, int& y, int cx, int cy, float r); //helper function for RotateRect

//Vector2D class:
Vector2D::Vector2D() {
    x = 0;
    y = 0;
}

Vector2D::Vector2D(int x, int y) {
    this->x = x;
    this->y = y;
}

Vector2D::Vector2D(point p) {
    x = p.x;
    y = p.y;
}

int Vector2D::dot(Vector2D& other) {
    return (x*other.x) + (y*other.y);
}

point Vector2D::to_point() {
    point p;
    p.x = x;
    p.y = y;
    return p;
}

//Rect2D class:
std::vector<Vector2D> Rect2D::get_points() {
    std::vector<Vector2D> points;
    points.push_back(top_right);
    points.push_back(top_left);
    points.push_back(bottom_right);
    points.push_back(bottom_left);
    return points;
}


/**
 * @brief Calculate the individual points of the physics bounding box of a Squiggly Object
 * 
 * @param obj 
 * @return Rect2D 
 */
Rect2D Physics::ObjBoundingBox(BuiltIn::Object& obj) 
{
    Rect2D box;

    //temp variables to make code a little cleaner
    int cx = obj.getX();
    int cy = obj.getY();
    int width = obj.getWidth() / 2;
    int height = obj.getHeight() / 2;

    int up = cy + height;
    int down = cy - height;
    int left = cx - width;
    int right = cx + width;

    //assigning values to bounding box struct
    box.bottom_left.x = left;
    box.bottom_left.y = down;

    box.bottom_right.x = right;
    box.bottom_right.y = down;

    box.top_left.x = left;
    box.top_left.y = up;
    
    box.top_right.x = right;
    box.top_right.y = up;

    box.center.x = cx;
    box.center.y = cy;

    //rotate box
    RotateRect(box, obj.getRotation());

    return box;
}

bool Physics::PointInRect(point& p, Rect2D& rect)
{
    //get the points in the polygon and use barycentric weights to check if the point is in either polygon
    point one = rect.bottom_left.to_point();
    point two = rect.top_left.to_point();
    point three = rect.top_right.to_point();
    point four = rect.bottom_right.to_point();

    //thank you Jake Muzyka for the idea to use barycentric weights for collision detection
    return PointInPollygon(one, two, three, p) || PointInPollygon(one, four, three, p);
}

bool Physics::PointInRect(Vector2D& p, Rect2D& rect)
{
    point pt;
    pt.x = p.x;
    pt.y = p.y;
    return PointInRect(pt, rect);
}

bool Physics::PointInPollygon(point& one, point& two, point& three, point& p) 
{
    // Compute the area of the triangle (using the determinant)
    float denom = (two.y - three.y) * (one.x - three.x) + (three.x - two.x) * (one.y - three.y);

    //  No divide by 0 or very small triangles
    if (std::abs(denom) < 1e-5f)
        return false;

    float w0, w1, w2;

    // Compute the barycentric weights
    w0 = ((two.y - three.y) * (p.x - three.x) + (three.x - two.x) * (p.y - three.y)) / denom;
    w1 = ((three.y - one.y) * (p.x - three.x) + (one.x - three.x) * (p.y - three.y)) / denom;
    w2 = 1.0f - w0 - w1;

    //  If weights are all valid, inside triangle
    if(w0 >= 0 && w1 >= 0 && w2 >= 0)
        return true;

    return false;
}

/**
 * @brief Rotate the points in a rectangle rot degrees
 * 
 * @param rect 
 * @param rot 
 */
void Physics::RotateRect(Rect2D& rect, float rot)
{
    rotate_point(rect.bottom_left.x, rect.bottom_left.y, rect.center.x, rect.center.y, rot);
    rotate_point(rect.bottom_right.x, rect.bottom_right.y, rect.center.x, rect.center.y, rot);
    rotate_point(rect.top_left.x, rect.top_left.y, rect.center.x, rect.center.y, rot);
    rotate_point(rect.top_right.x, rect.top_right.y, rect.center.x, rect.center.y, rot);
}

//helper functions
void rotate_point(int& x, int& y, int cx, int cy, float r) 
{
    r = r * (3.1415926 / 180); //degrees to radians
    
    float s = sin(r);
    float c = cos(r);

    // translate point back to origin:
    x -= cx;
    y -= cy;

    // rotate point
    float xnew = x * c - y * s;
    float ynew = x * s + y * c;

    // translate point back:
    x = xnew + cx;
    y = ynew + cy;
}