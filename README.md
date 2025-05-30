# Squiggly

An [esoteric language](https://en.wikipedia.org/wiki/Esoteric_programming_language) designed for simple game development

## Build/Run instructions:

> Note: make sure all submodules have been pulled before building for the first time

* From the root directory of the project, create build folder: `mkdir build && cd build`
* Run cmake: `cmake ../`
* Build the project: `make`
* Run Squiggly with: `./squiggly <filename>`
* Install: `make install`

## *...why?*

- **Why did you make this project?** 
    - Good question! The goal of this project is to eventually have an interpreter capable of being easily ported to different devices (such as the handheld Raspberry Pi game console I made as a test). This allows anyone to easily develop and test game scripts on their own computer before downloading them to any device with Squiggly installed.
    - This language is designed around game development, so I believe it will be a unique way to develop simple games. I love using scripting languages to throw something cool together, and I hope that Squiggly will be a fun way to do just that.
- **Why should I use this project?**
    - This interpreter (when built and ran from the `main` branch) will open it's own graphical window when running a script where you can play and test games!
    - Squiggly was built with portability in mind, so games written in this langauge can be played on many different devices. 
    - If you like experimenting with crappy software, this is definitely a fun way to do so (in my unbiased opinion).
- **Why did you call it that?**
    - This project is named "squiggly" due to the different built-in functionalities provided by this language which are accessed by "squiggly" shapes (@, $, ^). More information about these symbols in the [syntax information file](test_scripts/syntax.md)
    - The project was going to be named "squiggle" but according to [the complete list of esoteric languages](https://esolangs.org/wiki/Language_list), the name "squiggle" is already taken.

## Example squiggly syntax:

Test code can be found in the [/test_scripts](/test_scripts/) folder, but the general syntax looks like the following:

``` Python
:VARS: {
    OBJECT test

    float xVel
    float yVel
}

:START: {
    xVel = 0
    yVel = 0

    test.x = 0
    test.y = 0
    test.width = 20
    test.height = 20

    test.setColor(173, 245, 66)
    test.setShape(@TRIANGLE)
    test.setSolid(true) #turn fill off
}

:UPDATE: {
	test.x += 40 * xVel * $DTIME
    test.y += 40 * yVel * $DTIME

    xVel += $JOYSTICK_X
    yVel -= $JOYSTICK_Y

    xVel *= 0.8
    yVel *= 0.8

    ^PRINT("X: " + test.x + "   Y: " + test.y + "    Rot: " + test.rotation)

    if($A_BTN) {
        test.rotation -= 5
        test.setSolid(true)
    }

    if($B_BTN) {
        test.rotation += 5
        test.setSolid(false)
    }

    test.draw()
}
```