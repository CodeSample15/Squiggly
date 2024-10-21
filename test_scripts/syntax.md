# Rules and quirks of Squiggly:

> Here will be a list of known rules and invalid syntax with squiggly. As the language evolves, some of these rules may change, so it's important to update this list as the language gets updated

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

- 'else if' MUST be on one line, cannot do:
```cpp
    else
    if(STATEMENT)
```
- The tokenizer checks ONLY the same line as the `else` keyword for an `if`. Program will error if it is done the way it is shown above
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

## Loops:

- Squiggly uses the `repeat` function for looping:

```cpp
repeat(AMOUNT) {
    #looped code goes here
}
```

- The loop functions have the same brace rules as `if` statements (code can't be on the same line as an open or close brace `{}`)
- `repeat` will evaluate the argument passed as `AMOUNT` once, and loop that amount of times

> TODO: Add support for while loops

## Special characters:
**#** : Comment. Similar to Python. Squiggly (as of right now) does not support multi-line comments

**@** : Built in image reference. Squiggly will look for built-images/shapes with the name that follows the symbol (all caps)
    - Example: `@TRIANGLE`

**$** : Built in variable/function reference. Squiggly will provide functions and values to the program and will automatically update them, handling all input/output (names are also all caps)
    - Example: `$JOYSTICK_X` --> value of the game joystick 'x' value (between -1.0 and 1.0)
