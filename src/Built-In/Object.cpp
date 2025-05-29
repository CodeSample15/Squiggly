#include <cstdint>
#include <stdexcept>
#include <memory>
#include <string>
#include <algorithm>

#include "built-in.hpp"
#include "linter.hpp"
#include "runner.hpp"
#include "screen.hpp"
#include "utils.hpp"
#include "physics.hpp"

using namespace BuiltIn;

bool valueInRange(int val, int one, int two); //helper method for detecting collisions
void throwObjectError(std::string message);

BuiltIn::Object::Object() 
{
    //location
    x.name = "x";
    x.type = Utils::VarType::FLOAT;
    x.ptr = Utils::createEmptyShared(Utils::VarType::FLOAT);

    y.name = "y";
    y.type = Utils::VarType::FLOAT;
    y.ptr = Utils::createEmptyShared(Utils::VarType::FLOAT);

    //size
    width.name = "width";
    width.type = Utils::VarType::FLOAT;
    width.ptr = Utils::createEmptyShared(Utils::VarType::FLOAT);

    height.name = "height";
    height.type = Utils::VarType::FLOAT;
    height.ptr = Utils::createEmptyShared(Utils::VarType::FLOAT);

    //orientation (not yet implemented)
    rotation.name = "rotation";
    rotation.type = Utils::VarType::FLOAT;
    rotation.ptr = Utils::createEmptyShared(Utils::VarType::FLOAT);

    //color
    color_r.name = "color_r";
    color_r.type = Utils::VarType::INTEGER;
    color_r.ptr = Utils::createEmptyShared(Utils::VarType::INTEGER);

    color_g.name = "color_g";
    color_g.type = Utils::VarType::INTEGER;
    color_g.ptr = Utils::createEmptyShared(Utils::VarType::INTEGER);

    color_b.name = "color_b";
    color_b.type = Utils::VarType::INTEGER;
    color_b.ptr = Utils::createEmptyShared(Utils::VarType::INTEGER);

    shape = ObjectShape::RECT; //default object shape
    solid = true;

    //get variables from memory (flags set by functions)
    std::string flagName = COLLISION_FLAG_VAR_NAME;
    flagName.insert(0, 1, BUILT_IN_VAR_PREFIX);
    Utils::SVariable* tmp =  Runner::fetchVariable(flagName);
    if(tmp)
        collisionFlag = (bool*)tmp->ptr.get();
    else
        throwObjectError("Unable to fetch collision flag!"); //error that should never be seen by regular Squiggly users, just devs

    //default color (pink)
    setColor(255, 0, 255);

    //default values
    setWidth(OBJ_DEF_WIDTH);
    setHeight(OBJ_DEF_HEIGHT);
}

float BuiltIn::Object::getX() {
    return *(float*)x.ptr.get();
}

float BuiltIn::Object::getY() {
    return *(float*)y.ptr.get();
}

float BuiltIn::Object::getWidth() {
    return *(float*)width.ptr.get();
}

float BuiltIn::Object::getHeight() {
    return *(float*)height.ptr.get();
}

float BuiltIn::Object::getRotation() {
    return *(float*)rotation.ptr.get();
}

void BuiltIn::Object::setX(float v) {
    *(float*)x.ptr.get() = v;
}

void BuiltIn::Object::setY(float v) {
    *(float*)y.ptr.get() = v;
}

void BuiltIn::Object::setWidth(float v) {
    *(float*)width.ptr.get() = v;
}

void BuiltIn::Object::setHeight(float v) {
    *(float*)height.ptr.get() = v;
}

void BuiltIn::Object::setRotation(float v) {
    *(float*)rotation.ptr.get() = v;
}

void BuiltIn::Object::getColor(uint8_t buffer[3]) {
    buffer[0] = *(uint8_t*)color_r.ptr.get();
    buffer[1] = *(uint8_t*)color_g.ptr.get();
    buffer[2] = *(uint8_t*)color_b.ptr.get();
}

void BuiltIn::Object::callFunction(std::string name, std::vector<std::string>& args) 
{
    if(name == "draw") {
        draw();
    }
    else if(name == "testCollision") {
        if(args.size() != 1)
            throwObjectError("'testCollision' expected 1 argument, got " + std::to_string(args.size()));

        Utils::SVariable* tmp = Runner::fetchVariable(args[0]);
        if(tmp && tmp->type == Utils::VarType::OBJECT) {
            Object* other = (Object*)tmp->ptr.get();
            *collisionFlag = isTouching(*other); //set collision flag 
        } else {
            throwObjectError("'testCollision' -> '" + args[0] + "' is not an Object variable");
        }
    }
    else if(name == "collide") {
        if(args.size() != 1)
            throwObjectError("'collide' expected 1 argument, got " + std::to_string(args.size()));

        Utils::SVariable* tmp = Runner::fetchVariable(args[0]);
        if(tmp && tmp->type == Utils::VarType::OBJECT) {
            //collide the two objects using the physics library
            Object* other = (Object*)tmp->ptr.get();
            collideWith(*other);
        } else {
            throwObjectError("'collide' -> '" + args[0] + "' is not an Object variable");
        }
    }
    else if(name == "setColor") {
        if(args.size() != 3)
            throwObjectError("'setColor' expected 3 arguments, got "+ std::to_string(args.size()));

        //convert arguments to literal values
        int r = *(int*)Utils::convertToVariable(args[0], Utils::VarType::INTEGER).ptr.get();
        int g = *(int*)Utils::convertToVariable(args[1], Utils::VarType::INTEGER).ptr.get();
        int b = *(int*)Utils::convertToVariable(args[2], Utils::VarType::INTEGER).ptr.get();

        //avoid under/overflow
        r = std::clamp(r, 0, 255);
        g = std::clamp(g, 0, 255);
        b = std::clamp(b, 0, 255);

        //call builtin method
        setColor(r, g, b);
    }
    else if(name == "setShape") {
        if(args.size() != 1)
            throwObjectError("'setShape' expected 1 argument, got " + std::to_string(args.size()));
        if(args[0][0] != IMAGE_DECLARATION_PREFIX)
            throwObjectError("Argument passed to 'setImage' is not an image declaration! (Should start with @)");

        //remove prefix
        args[0] = args[0].substr(1, args[0].length() - 1);
        setObjShape(args[0]);
    }
    else if(name == "setSolid") {
        if(args.size() != 1)
            throwObjectError("'setSolid' expected 1 argument, got " + std::to_string(args.size()));

        //set internal variable
        solid = *(bool*)Utils::convertToVariable(args[0], Utils::VarType::BOOL).ptr.get();
    }
    else {
        throwObjectError("Function name \'" + name + "\' does not exist.");
    }
}

Utils::SVariable* BuiltIn::Object::fetchVariable(std::string name) 
{
    if(name=="x")
        return &x;
    else if(name=="y")
        return &y;
    else if(name=="width")
        return &width;
    else if(name=="height")
        return &height;
    else if(name=="rotation")
        return &rotation;
    else if(name=="color_r")
        return &color_r;
    else if(name=="color_g")
        return &color_g;
    else if(name=="color_b")
        return &color_b;

    throwObjectError("Object variable \'" + name + "\' not found!");
    return nullptr; //not likely to run at all, but putting it here anyway
}

void BuiltIn::Object::draw() 
{
    screen.drawObj(*this);
}

bool BuiltIn::Object::isTouching(Object& other) 
{
    //get the bounding boxes for each object
    Physics::Rect2D one = Physics::ObjBoundingBox(*this);
    Physics::Rect2D two = Physics::ObjBoundingBox(other);

    //check if any point of either objects is within each other
    //pretty sure this is super bulky and inefficient, so TODO: make this algorithm the SAT collision algorithm
    for(Physics::Vector2D& point : one.get_points()) {
        if(Physics::PointInRect(point, two))
            return true;
    }

    for(Physics::Vector2D& point : two.get_points()) {
        if(Physics::PointInRect(point, one))
            return true;
    }

    return false;
}

void BuiltIn::Object::collideWith(Object& other) 
{
    //get bounding boxes for both objects
    Physics::Rect2D thisBox = Physics::ObjBoundingBox(*this);
    Physics::Rect2D otherBox = Physics::ObjBoundingBox(other);

    Physics::MovePointOutOfRect(thisBox.top_left, otherBox);

    //find points inside of another object

    // for(Physics::Vector2D& point : thisBox.get_points()) {
    //     if(Physics::PointInRect(point, otherBox)) {

    //     }
    // }

    // for(Physics::Vector2D& point : otherBox.get_points()) {
    //     if(Physics::PointInRect(point, thisBox)) {

    //     }
    // }    

    //for each point inside of an object:
        //find point which is closest to a border
        //move object back distance 

    //find points which are in other object

    //apply bounding box to object
    // setX(thisBox.center.x);
    // setY(thisBox.center.y);
}

void BuiltIn::Object::setObjShape(std::string img) 
{
    if(img == "TRIANGLE")
        shape = ObjectShape::TRIANGLE;
    else if(img == "RECT")
        shape = ObjectShape::RECT;
    else if(img == "ELLIPSE")
        shape = ObjectShape::ELLIPSE;
}

/*
    This is just here mainly to make setting the color of the object through c++ code much easier to do
*/
void BuiltIn::Object::setColor(uint8_t r, uint8_t g, uint8_t b) {
    *(int*)color_r.ptr.get() = r;
    *(int*)color_g.ptr.get() = g;
    *(int*)color_b.ptr.get() = b;
}

bool valueInRange(int val, int one, int two) {
    return (val >= one && val <= two) || (val <= one && val >= two);
}

void throwObjectError(std::string message) {
    throw std::runtime_error("Built in object error : " + message);
}