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
            throwObjectError("'isTouching' -> '" + args[0] + "' is not an Object variable");
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

        args[0] = args[0].substr(1, args[0].length() - 1);
        setObjShape(args[0]);
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
    bool xOverlap = valueInRange(getX(), other.getX(), other.getX()+other.getWidth())
                || valueInRange(other.getX(), getX(), getX()+getWidth());

    bool yOverlap = valueInRange(getY(), other.getY(), other.getY()+other.getHeight())
                || valueInRange(other.getY(), getY(), getY()+getHeight());
    
    return xOverlap && yOverlap;
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