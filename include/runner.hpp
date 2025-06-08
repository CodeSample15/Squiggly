#pragma once

#include "utils.hpp" 

//built in variable names
#define JOYSTICK_X_VAR_NAME "JOYSTICK_X"
#define JOYSTICK_Y_VAR_NAME "JOYSTICK_Y"
#define BUTTON_A_VAR_NAME "A_BTN"
#define BUTTON_B_VAR_NAME "B_BTN"

#define FPS_VAR_NAME "FPS"
#define DTIME_VAR_NAME "DTIME"

#define SCREEN_WIDTH_VAR_NAME "SCREEN_WIDTH"
#define SCREEN_HEIGHT_VAR_NAME "SCREEN_HEIGHT"

#define COLLISION_FLAG_VAR_NAME "COL_FLAG"
#define FLOAT_RETURN_BUCKET_VAR_NAME "F_RET" //bypass the whole "squiggly can't return values from functions" issue. Float values returned from functions can be stored in this bucket variable
#define INT_RETURN_BUCKET_VAR_NAME "I_RET"

//important constants
#define MEM_RESERVE_SIZE 1000

namespace Runner {
    //delete all old values stored in memory
    void flushMem();
    
    //run each segment of the code once
    void executeVars();
    void executeStart();
    void executeUpdate();

    //run the whole program and loop executeUpdate
    void execute();

    //allow external cpp files to access variables
    Utils::SVariable* fetchVariable(std::string varName, bool allowArrays=false);
}