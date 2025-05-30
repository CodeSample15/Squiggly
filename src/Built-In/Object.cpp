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

void throwObjectError(std::string message);

size_t BuiltIn::Object::obj_count = 0;

BuiltIn::Object::Object() 
{
    //assign unique ID value
    id = obj_count++;

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

    walls.clear(); //no reason why this should have anything in it, but hey what the heck clear it anyway

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
    else if(name == "move") {
        if(args.size() != 2 && args.size() != 3)
            throwObjectError("'move' expected 2 or 3 arguments, got " + std::to_string(args.size()));

        float x = *(float*)Utils::convertToVariable(args[0], Utils::VarType::FLOAT).ptr.get();
        float y = *(float*)Utils::convertToVariable(args[1], Utils::VarType::FLOAT).ptr.get();
        bool collide = false;
        if(args.size() == 3)
            collide = *(bool*)Utils::convertToVariable(args[2], Utils::VarType::BOOL).ptr.get();
        
        move(x, y, collide);
    }
    else if(name == "addWall") {
        //add an object that this object will be a wall for (other object can't pass through)
        if(args.size() != 2)
            throwObjectError("'addWall' expected 2 arguments, got " + std::to_string(args.size()));

        Utils::SVariable* tmp = Runner::fetchVariable(args[0]);
        if(tmp && tmp->type == Utils::VarType::OBJECT) {
            Object* other = (Object*)tmp->ptr.get();
            
            //get other argument
            bool add = *(bool*)Utils::convertToVariable(args[1], Utils::VarType::BOOL).ptr.get();
            addWall(other, add);
        } else {
            throwObjectError("'addWall' -> '" + args[0] + "' is not an Object variable");
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

void BuiltIn::Object::setObjShape(std::string img) 
{
    if(img == "TRIANGLE")
        shape = ObjectShape::TRIANGLE;
    else if(img == "RECT")
        shape = ObjectShape::RECT;
    else if(img == "ELLIPSE")
        shape = ObjectShape::ELLIPSE;
}

/**
 * @brief Move an object in 2D space. Optionally collide with barriers
 * 
 * @param x
 * @param y
 * @param collide
 */
void BuiltIn::Object::move(float x, float y, bool collide)
{
    float oldX = getX();
    float oldY = getY();

    float newX = oldX+x;
    float newY = oldY+y;

    setX(newX);
    setY(newY);

    if(collide) {
        //calculate normalized vector of position change for amount to move player
        Physics::Vector2D diff(newX-oldX, newY-oldY);
        Physics::Vector2D segmented_movement = diff / OBJ_COL_RESP_SEGMENTS; //divide the area between the new and old positions into discrete sections to check for a resolution to a collision
        Physics::Vector2D tempDelta;
        int currSegment = OBJ_COL_RESP_SEGMENTS; //this value will decrease for each position checked

        //check all objects which are walls for collisions
        for(Object* wall : walls) {
            while(currSegment > 0 && isTouching(*wall)) {
                currSegment--;
                tempDelta = segmented_movement*currSegment; //calculate where the object needs to move from new location

                newX = oldX + tempDelta.x;
                newY = oldY + tempDelta.y;

                setX(newX);
                setY(newY);
            }
        }
    }
}

/**
 * @brief Make this object a wall for the other object
 * 
 * @param other
 * @param add
 * If add is true, add object to wallForIDs, if false, remove item if it's already in the list
 */
void BuiltIn::Object::addWall(Object* wall, bool add) 
{
    for(int i=0; i<walls.size(); i++) {
        if(walls[i]->id == wall->id) {
            if(add)
                return;
            else {
                walls.erase(walls.begin()+i);
                return;
            }
        }
    }

    walls.push_back(wall);
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