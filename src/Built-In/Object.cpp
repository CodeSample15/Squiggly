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
    x.name = "x";
    x.type = Utils::VarType::FLOAT;
    x.ptr = Utils::createEmptyShared(Utils::VarType::FLOAT);

    y.name = "y";
    y.type = Utils::VarType::FLOAT;
    y.ptr = Utils::createEmptyShared(Utils::VarType::FLOAT);

    width.name = "width";
    width.type = Utils::VarType::FLOAT;
    width.ptr = Utils::createEmptyShared(Utils::VarType::FLOAT);

    height.name = "height";
    height.type = Utils::VarType::FLOAT;
    height.ptr = Utils::createEmptyShared(Utils::VarType::FLOAT);

    rotation.name = "rotation";
    rotation.type = Utils::VarType::FLOAT;
    rotation.ptr = Utils::createEmptyShared(Utils::VarType::FLOAT);

    shape = ObjectShape::RECT;

    //default color (pink)
    color[0] = 255;
    color[1] = 0;
    color[2] = 255;
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

    throwObjectError("Object variable \'" + name + "\' not found!");
    return nullptr; //not likely to run at all, but putting it here anyway
}

void BuiltIn::Object::draw() 
{
    screen.drawObj(*this);
}

void throwObjectError(std::string message) {
    throw std::runtime_error("Built in object error : " + message);
}