/*
    This file contains handles for each display function needed to make Squiggly work

    The aim of this file is to loosly couple the frontend side of Squiggly so that, when porting
    the language to arduino, the function names used by other parts of the code can remain the
    same while the actual implementation can be treated differently.

    For the PC version of Squiggly, games will be rendered to an opencv window and game input will
    be taken from the keyboard
*/