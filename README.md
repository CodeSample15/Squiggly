# Squiggly

An [esoteric language](https://en.wikipedia.org/wiki/Esoteric_programming_language) designed to be a quick way to develop games using scripts

> WARNING: This project is still in development and does not function as intended.

> ANOTHER WARNING: Until this message is deleted from the README, the squiggly language will be considered unstable, meaning it's syntax may change and adapt over time as it gets updated. Keep this in mind when writing scripts as you might need to change your syntax if you plan on updating.

## Build/Run instructions:

> Important: In order to compile for Windows, you must have the OpenCV library installed on your device. If you don't, you can find the instructions [here](https://youtu.be/m9HBM1m_EMU)

* From the root directory of the project, create build folder: `mkdir build && cd build`
* Run cmake: `cmake -G "Unix Makefiles" ../`
* Build the project: `make`
* Run Squiggly with: `./squiggly <path to .sqgl file>`
* Install: `make install`

## *...why?*

- **Why did you make this project?** 
    - Good question! The goal of this project is to eventually have an interpreter capable of being easily ported to any embedded system, allowing anyone to easily develop game scripts that can be tested on their computer before being downloaded to a portable player.
    - This language is designed around game development, so I believe it will be a unique way to develop simple games. I love using scripting languages to throw something cool together, and I hope that Squiggly will be a fun way to do just that.
- **Why can't I make games with this yet?**
    - As of right now, Squiggly is only capable of handling basic input (in Windows using the arrow keys and 'z' and 'x' keys), and providing basic output (using the ^PRINT function). I am currently working on the graphical end of Squiggly, and hope to get something simple working soon.
- **Why should I use this project?**
    1. This interpreter (when built and ran from the `main` branch) will open it's own graphical window when running a script where you can play and test games!
    2. When this interpreter is ported over to embedded systems, it will allow those same scripts to be played on any portable mini-consoles built using arduino (and raspberry pis in the future)!
    3. If you like experimenting with crappy software, this is definitely a fun way to do so (in my unbiased opinion).
- **Why did you call it that?**
    - This project is named "squiggly" due to the different built-in functionalities provided by this language which are accessed by "squiggly" shapes (@, $, ^). More information about these symbols in the [syntax information file](test_scripts/syntax.md)
    - The project was going to be named "squiggle" but according to [the complete list of esoteric languages](https://esolangs.org/wiki/Language_list), the name "squiggle" is already taken.

## Example squiggly syntax:

Test code can be found in the [/test_scripts](/test_scripts/) folder, but the general syntax looks like the following:

``` Python
:VARS: {
    float damage = 0.5

    OBJECT player
    OBJECT enemy
    OBJECT[] enemies

    #comment example
    TEXT scoreText
}

:START: {
    int score = 0
    int deaths = 0

    player.icon = @CIRCLE
    enemy.icon = @TRIANGLE

    if (damage == 1) {
        deaths += 1
    }

    repeat(5) {
        score += 1
    }
}

:UPDATE: {
	player.x += $JOYSTICK_X
	player.y += $JOYSTICK_Y
}

spawnEnemy() {
	enemies.add(CREATE_NEW(enemy))
}
```