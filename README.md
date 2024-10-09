# Squiggly

An esoteric language designed to be a quick way to develop games using scripts

> WARNING: This project is still in development and does not function.

> ANOTHER WARNING: Until this message is deleted from the README, the squiggly language will be considered unstable, meaning it's syntax may change and adapt over time as it gets updated. Keep this in mind when writing scripts as you might need to change your syntax if you plan on updating.

## Build/Run instructions:

* From the root directory of the project, run `make`
* To run: `./squiggly <FILENAME>`

## *...why?*

- **Why did you make this project?** 
    - Good question! The goal of this project is to eventually have an interpreter capable of being easily ported to any embedded system, allowing for a way for people to easily develop game scripts that can be ran and tested on their computer before being downloaded to a portable player.
- **Why doesn't this do anything?**
    - Cuz I'm not done yet. Please wait until this message doesn't appear in the README.
- **Why should I use this project?**
    1. This interpreter (when built and ran from the `main` branch) will open it's own graphical window when running a script where you can play and test games!
    2. When this interpreter is ported over to embedded systems, it will allow those same scripts to be played on any portable mini-consoles built using arduino (and raspberry pis in the future)!
    3. If you like experimenting with crappy software, this is definitely a fun way to do so (in my unbiased opinion).
- **Why did you call it that?**
    - This project is named "squiggly" due to it's syntax looking kind of squiggly. That's really the only reason.
    - The project was going to be named "squiggle" but according to [the complete list of esoteric languages](https://esolangs.org/wiki/Language_list), the name "squiggle" is already taken.

## Example squiggly syntax:

Test code can be found in the `/test_scripts` folder, but the general syntax looks like the following:

``` Python
:VARS: {
    score
    deaths

    float damage = 0.5

    OBJECT player
    OBJECT enemy
    OBJECT[] enemies

    #comment example
    TEXT scoreText
}

:START: {
	score = 0
	deaths = 0

	player.icon = @CIRCLE
	enemy.icon = @TRIANGLE

    if (damage == 0.5) {
        
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