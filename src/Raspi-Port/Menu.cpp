#include <stdlib.h>
#include "Raspi-Port/Menu.hpp"

void readFiles(std::vector<std::string>& paths);
void draw_menu(std::vector<std::string>& paths, int selection);

float menu_offset = 0; //for sliding animation of menu
float menu_speed = 0.1; //menu move speed
float min_menu_dist = 0.1; //snap to location after this minimum distance from target offset
float target_menu_loc = 0;

std::string run_menu(std::vector<std::string>& fileLines) {
    //start screen and get screen pointer
    Frontend::init();
    ST7735_TFT* screen = Frontend::get_tft();
    screen->IMClear();
    screen->TFTFontNum(screen->TFTFont_Default);
    screen->TFTsetRotation(screen->TFT_rotate_e::TFT_Degrees_90); //rotate screen

    //fetch paths from file system
    std::vector<std::string> paths;
    readFiles(paths);

    //for determining what the user wants
    int selection = 0;
    bool selection_made = false;
    std::string path = "";

    bool pressed = false;
    while(!selection_made) {
        do {
            draw_menu(paths, selection); //continue to draw screen each loop (allows for animations to play)
            Frontend::updateReadings();
            TFT_MILLISEC_DELAY(10);
        } while(pressed && Frontend::getVertAxis()!=0); //wait until there is no more input

        pressed = false;

        //input from joystick
        if(Frontend::getVertAxis() == 1) {
            selection++;
            if(selection>=paths.size())
                selection = 0;
            pressed = true;
        } 
        else if(Frontend::getVertAxis() == -1) {
            selection--;
            if(selection<0)
                selection = paths.size()-1;
            pressed = true;
        } else if(Frontend::getABtn()) {
            path = paths[selection];
            break;
        }
    }

    //reset screen rotation
    screen->TFTsetRotation(screen->TFT_rotate_e::TFT_Degrees_0);

    return path;
}

void draw_menu(std::vector<std::string>& paths, int selection) {
    size_t path_len;
    std::string filename;

    //update offset
    menu_offset += (target_menu_loc - menu_offset) * menu_speed;
    if(abs(menu_offset-target_menu_loc) < min_menu_dist)
        menu_offset = target_menu_loc;

    for(int i=0; i<paths.size(); i++) {
        //extract the name of the script from the path
        path_len = std::string(SCRIPT_PATH).length();
        filename = paths[i].substr(path_len, paths[i].length-path_len);

        //calculate if we are still rendering on screen (avoid overflow/underflow when calculating text position)

        screen->TFTdrawText(TEXT_LEFT_BUFFER, ((uint8_t)i*TEXT_PIXEL_HEIGHT) - (uint8_t)menu_offset, (char*)filename.c_str(), ST7735_WHITE, ST7735_BLACK, 1, true); //draw text to the in memory buffer of the screen
    }

    //todo: draw line under selected path
    screen->TFTdrawFastHLine(TEXT_LEFT_BUFFER, ((uint8_t)i*TEXT_PIXEL_HEIGHT) - (uint8_t)menu_offset + (uint8_t)(TEXT_PIXEL_HEIGHT/2), 2, 0xFFFF);

    screen->IMDisplay(); //render the in memory buffer to the physical screen
}

void readFiles(std::vector<std::string>& paths) {
    paths.push_back(SCRIPT_PATH + "test");
    paths.push_back(SCRIPT_PATH + "another test");
    paths.push_back(SCRIPT_PATH + "Menu test");
    paths.push_back(SCRIPT_PATH + "Menu Item");
}