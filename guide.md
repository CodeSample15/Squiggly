# Getting Started With Squiggly

> Squiggly is inspired from languages like Java, C++, and Python. If you've programmed in any of these languages before, Squiggly should seem somewhat familiar. You will also find, however, some limitations to Squiggly's syntax which mainly come from the basic tokenization method I've decided to use for this project. In this file I will cover most of these limitations, and will update the contents as I discover more potential issues.

<br/>

## Basics

Running `squiggly template` will print a project template to the console which you can copy paste into any file:

```
:VARS: {
    # global variables go here
}

:START: {
    # initialization code goes here
}

:UPDATE: {
    # game loop logic goes here
}
```

> Notes:
> 
> Squiggly will not run any code put outside of these 3 functions. The only code that will be written outside of these function blocks will be user-defined function declarations.
>
> Squiggly will not run if it does not detect all three of these functions in the file, meaning these functions MUST be included, even if they are empty

**Template overview**:
 - :VARS:
    - Variables declared in this block will remain in scope throughout the entire program. You can write normal Squiggly code in this block, but the best practice is to limit code written here to just variable declaration
- :START:
    - This block of code is second to execute (runs after :VARS: block). Code written here will run only once, so it's useful for initialization. 
    - *An important thing to note* is that access to built-in variables in both this block and the :VARS: block is allowed, however some of the values may not be set to any value yet (FPS, D_TIME, JOYSTICK_X/Y, etc) and will be 0 by default. Variables like SCREEN_WIDTH/HEIGHT will be initialized and can be used as normal.
- :UPDATE:
    - Here is where game loop logic lives. Code put here will run once per frame until the game is stopped.

<br/>

## Special characters:
- **#** : Comment. Similar to Python. Squiggly (as of right now) does not support multi-line comments

- **@** : Built in image reference. Squiggly will look for built-images/shapes with the name that follows the symbol (all caps)
     - Example: `@TRIANGLE`

- **$** : Built in variable reference. Squiggly will provide values to the program and will automatically update them, handling all input to the program (names are also all caps)
     - Example: `$JOYSTICK_X` --> value of the game joystick 'x' value (between -1.0 and 1.0)

- **^** : Built in function reference. Squiggly will provide built in functions for the user to be able to create output from their program (function names are all caps)
     - Example: ^PRINT("Hello World!")

<br/>

## If-else statements:

- If-else statements should be structured in one of two ways:
```cpp
    if(STATEMENT) {

    } else {

    }
```
**OR**
```cpp
    if(STATEMENT) {

    }
    else {

    }
```
- This is because the tokenizer will check the same line or the line immediately after the closing brace of an if statement for an `else`

- 'else if' MUST be on one line, the following **WILL NOT** work:
```cpp
    else
    if(STATEMENT)
```
- The tokenizer checks ONLY the same line as the `else` keyword for an `if`. Squiggly will error if it is done the way it is shown above
- Braces can be placed either on the same line, or the line immediately next after the if statement:
```cpp
    if(STATEMENT) {
```
```cpp
    if(STATEMENT)
    {
```
- No code can be placed on the same line as an open or close brace. So, the following is **NOT** allowed:
```cpp
    if(STATEMENT) { i++ }
```
```python
    i=5 } #this line won't run
```
- Lastly, Squiggly **does not** accept if-statements without braces. The following will also **not** work:
```python
    if(statement)
        runFunction()
```

<br/>

## Supported operations

Squiggly uses the [exprtk library](https://github.com/ArashPartow/exprtk) to parse mathmatical and boolean statements. Below is a list of operations which Squiggly supports with the use of this library:

### Mathmatics:
These are the ONLY functions in Squiggly which return values in-line:
- `sqrt()`
- `ceil()`
- `cos()`
- `sin()`
- `tan()`
- `abs()`

### Boolean:
> Note: Squiggly does not support `&&`, `||`, or `!` for boolean expressions. Only the following are supported:
- `and`
- `or`
- `not`
- `xor`

<br/>

## Loops:

> Loop functions have the same brace rules as `if` statements (code can't be on the same line as an open or close brace `{}`, braces must exist around looped code, etc)

### *repeat*

```cpp
repeat(AMOUNT) {
    #looped code goes here
}
```

- `repeat` will evaluate the argument passed as `AMOUNT` once, and loop that amount of times
- AMOUNT is auto casted into an integer after being evaluated

### *while*

```python
while(BOOLEAN) {
    #looped code goes here
}
```

- `while` works just like while loops in other programming languages. Anything passed as the boolean statement will evaluate each time the loop is ran.

<br/>

## Variables

Squiggly supports the following datatypes which work similar to other languages:
```c++
int i #integer values
float f #float values
double # double values
bool b #boolean values
string s #string literals
string stwo = "example"
```

In addition to these "primitive" data types, Squiggly come built in with an additional object data type:

```
OBJECT player
```

All variable types can be turned into 1D arrays by following the following syntax when declaring a variable:
```python
int i[LENGTH] # replace LENGTH with the length that you wish your array to be (auto casted to an integer)
```
> Note: due to another limitation with Squiggly's tokenizer, multidimensional arrays are not currently supported.

Referencing items in an array is similar to other popular languages:
``` python
i[INDEX] # replace INDEX with the index you wish to reference in your array
```


<br/>

## The OBJECT variable

The OBJECT variable allows for the creation of a controllable icon which can be drawn in the game window. Check out [this script](/test_scripts/objectTest.sqgly) to see an example of how OBJECTs can be used.

OBJECTs have the following variables which can be assigned and referenced as any other variable in Squiggly:
```
OBJECT test
test.x
test.y
test.width
test.height
test.rotation
test.color_r
test.color_g
test.color_b
```

<br/>

### OBJECT functions

- `.draw()`
    - Draws object onto the game screen
    - Must be called each frame you wish to draw the object (screen is automatically cleared after each gameloop)
- `.move(float x, float y, bool collide=false)`
    - Move the object by x and y amount. Arguments are added to current x and y position
    - Optionally collide with objects that are marked for collision (see `.addWall`)
        - If collide==true, object will not pass through walls when moving
- `.setColor(int r, int g, int b)`
    - Set the color of the object (values should be 0-255)
- `.setShape(SHAPE)`
    - Set the shape of the object. Shapes are referenced in Squiggly by using the `@` symbol. Currently, only three different shapes are supported:
        - `@TRIANGLE`
        - `@RECT`
        - `@ELLIPSE`
            - > Note for ellipse: control over height and width is not yet supported. The ellipse shape draws a circle which can change size using only the `.width` property of the object.
- `.setSolid(bool solid)`
    - Set whether or not the object is drawn with fill (`.setSolid(false)` draws a hollow shape)
    - This function does not affect collision calculations
- `.testCollision(OBJECT other)`
    - Test to see if the object is touching `other`. 
    - Sets built-in variable `$COL_FLAG` as a return value (true for collision, false otherwise)
- `.addWall(OBJECT other, bool add=true)`
    - Registers `other` as a wall to the object (useful for `.move` with collision)
        - To remove `other` as a wall, pass false as the second argument
    - See [this script](/test_scripts/collisionHandlingTest.sqgly) for an example of this function's use

<br/>

## Built-in variables:

Below is the current list of built-in variables accessible in Squiggly programs. This should also be kept up to date as the project develops:

- `$JOYSTICK_X / $JOYSTICK_Y` **(float)**: analog values between -1.0 and 1.0 of the input provided to the controller used to play a Squiggly game (keyboard arrow keys for Windows build)
- `$A_BTN / $B_BTN` **(bool)**: additional input buttons to Squiggly programs (for Windows build: <u>A = Z keyboard button</u> and <u>B = X keyboard button</u>)
- `$FPS` **(int)**: current frames per second of the Squiggly window. Useful for debugging and benchmarking purposes
- `$DTIME` **(float)**: time between each frame. Useful for consistent value changes (like position) across different frame rates
- `$SCREEN_WIDTH / $SCREEN_HEIGHT` **(int)**: dimensions (in pixels) of the screen being drawn to
- `$COL_FLAG` **(bool)**: flag set by built in objects when .testCollision() is called (true if the two objects are touching, false otherwise)
- `$F_RET` **(float)**: float return bucket for functions to dump values in (workaround to the fact that Squiggly doesn't support functions which return values)
- `I_RET` **(int)**: int return bucket

<br/>

## Built-in functions:

> Built-in functions are referenced using the `^` symbol

- `^PRINT(string s)`
    - Print a string to the console. Useful for debugging.
- `^LEN(array)`
    - Passing an array to this function will store the length of the array in `$I_RET`
- `^I_RAND(int min, int max)`
    - Generate a pseudo random number in the range [min, max)
    - Sets `I_RET` with generated number
- `^F_RAND()`
    - Generate a pseudo random number in the range [0.0, 1.0)
    - Sets `F_RET` with generated number
- `^DRAW_LINE(float x1, float y1, float x2, float y2, int r=255, int g=255, int b=255)`
    - Draw a line to screen starting at (x1,y1) and ending at (x2,y2) with color (r,g,b)