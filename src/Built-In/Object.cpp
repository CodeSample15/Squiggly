#include <cstdint>
#include <stdexcept>
#include <memory>
#include <string>

#include "built-in.hpp"
#include "screen.hpp"
#include "utils.hpp"

using namespace BuiltIn;

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

    shape = ObjectShape::RECT; //TODO: allow other shapes to be used for object

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
    else if(name == "isTouching") {
        if(args.size() != 1)
            throwObjectError("'isTouching' expected 1 argument, got " + std::to_string(args.size()));

        //Object* other = 
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

/*
    This is just here mainly to make setting the color of the object through c++ code much easier to do
*/
void BuiltIn::Object::setColor(uint8_t r, uint8_t g, uint8_t b) {
    *(int*)color_r.ptr.get() = r;
    *(int*)color_g.ptr.get() = g;
    *(int*)color_b.ptr.get() = b;
}

void throwObjectError(std::string message) {
    throw std::runtime_error("Built in object error : " + message);
}