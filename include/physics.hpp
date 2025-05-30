#pragma once

#include <vector>
#include "built-in.hpp"

//simple 2D physics engine using box collision
namespace Physics {
    typedef struct {
        int x;
        int y;
    } point;

    class Vector2D {
        public:
            Vector2D();
            Vector2D(float x, float y);
            Vector2D(point p);

            float x, y;

            Vector2D operator+(Vector2D& other) {
                return Vector2D(x+other.x, y+other.y);
            }
            Vector2D operator-(Vector2D& other) {
                return Vector2D(x-other.x, y-other.y);
            }
            Vector2D operator*(float val) {
                return Vector2D(x*val, y*val);
            }
            Vector2D operator/(float val) {
                return Vector2D(x/val, y/val);
            }

            float length();
            int dot(Vector2D& other);
            Vector2D normalize();
            point to_point(); //convert vector to a point
    };

    class Rect2D {
        public:
            void translateX(int amount);
            void translateY(int amount);

            std::vector<Vector2D> get_points();

            Vector2D top_right;
            Vector2D top_left;
            Vector2D bottom_right;
            Vector2D bottom_left;
            Vector2D center;
    }; //to represent bounding boxes of objects

    Rect2D ObjBoundingBox(BuiltIn::Object& obj); //get the 2D rectangle which is the physics bounding box for an object

    bool PointInRect(point& p, Rect2D& rect); //detect collision
    bool PointInRect(Vector2D& p, Rect2D& rect);

    bool PointInPollygon(point& one, point& two, point& three, point& p);
    void RotateRect(Rect2D& rect, float rot); //rotate a Rect2D in place
}