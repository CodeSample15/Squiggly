# Rules and quirks of Squiggly:

> Here will be a list of known rules and invalid syntax with squiggly. As the language evolves, some of these rules may change, so it's important to update this list as the language gets updated



## Special characters:
**#** : Comment. Similar to Python. Squiggly (as of right now) does not support multi-line comments

**@** : Built in image reference. Squiggly will look for built-images/shapes with the name that follows the symbol (all caps)
    - Example: `@TRIANGLE`

**$** : Built in variable reference. Squiggly will provide values to the program and will automatically update them, handling all input to the program (names are also all caps)
    - Example: `$JOYSTICK_X` --> value of the game joystick 'x' value (between -1.0 and 1.0)

**^**: Built in function reference. Squiggly will provide built in functions for the user to be able to create output from their program (function names are all caps)
    - Example: ^PRINT("Hello World!")

## Built-in functions:

Below is the current list of built-in functions supported by Squiggly. This should be kept up to date as the project develops:

- `^PRINT(string message)`: Print a message to a debug terminal


## Built-in variables:

Below is the current list of built-in variables accessible in Squiggly programs. This should also be kept up to date as the project develops:

- `$JOYSTICK_X / $JOYSTICK_Y` **(float)**: analog values between -1.0 and 1.0 of the input provided to the controller used to play a Squiggly game (keyboard arrow keys for Windows build)
- `$A_BTN / $B_BTN` **(bool)**: additional input buttons to Squiggly programs (for Windows build: <u>A = Z keyboard button</u> and <u>B = X keyboard button</u>)
- `$FPS` **(int)**: current frames per second of the Squiggly window. Useful for debugging and benchmarking purposes
- `$DTIME` **(float)**: time between each frame. Useful for consistent value changes (like position) across different frame rates
- `$SCREEN_WIDTH / $SCREEN_HEIGHT` **(int)**: dimensions (in pixels) of the screen being drawn to
- `$COL_FLAG` **(bool)**: flag set by built in objects when .isTouching() is called (true if the two objects are touching, false otherwise)