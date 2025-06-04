# Getting Started With Squiggly

> Squiggly is inspired from languages like Java, C++, and Python. If you've programmed in any of these languages before, Squiggly should seem somewhat familiar. You will also find, however, some limitations to Squiggly's syntax which mainly come from the basic tokenization method I've decided to use for this project. In this file I will cover most of these limitations, and will update the contents as I discover more potential issues.

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

## Loops:

- Squiggly uses the `repeat` function for looping:

```cpp
repeat(AMOUNT) {
    #looped code goes here
}
```

- The loop functions have the same brace rules as `if` statements (code can't be on the same line as an open or close brace `{}`, braces must exist around looped code, etc)
- `repeat` will evaluate the argument passed as `AMOUNT` once, and loop that amount of times
- AMOUNT is auto casted into an integer after being evaluated

> TODO: Add support for while loops