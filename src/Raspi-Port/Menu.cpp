#include <stdlib.h>
#include <filesystem>

#include "Raspi-Port/Menu.hpp"
#include "screen.hpp"

namespace fs = std::filesystem;

void readFiles(std::vector<std::string>& paths);
void draw_menu(ST7735_TFT* screen, std::vector<std::string>& paths, size_t selection);

float menu_offset = 0;

std::string run_menu() {
    //start screen and get screen pointer
    Frontend::init();
    ST7735_TFT* screen = Frontend::get_tft();
    screen->IMClear();
    screen->TFTFontNum(screen->TFTFont_Default);
    screen->TFTsetRotation(screen->TFT_rotate_e::TFT_Degrees_270); //rotate screen

    //fetch paths from file system
    std::vector<std::string> paths;
    readFiles(paths);

    //for determining what the user wants
    size_t selection = 0;
    bool selection_made = false;
    std::string path = "";

    bool pressed = false;
    bool change = true;
    while(!selection_made) {
        //only redraw menu if there was a change (prevent wasted resources with redraw to TFT)
        if(change)
            draw_menu(screen, paths, selection);

        do {
            Frontend::updateReadings();
        } while(pressed && Frontend::getVertAxis()!=0); //wait until there is no more input

        pressed = false;
        change = false;

        //input from joystick
        if(Frontend::getVertAxis() == -1) {
            selection++;
            if(selection>=paths.size())
                selection = 0;
            pressed = true;
            change=true;
        } 
        else if(Frontend::getVertAxis() == 1) {
            if(selection<1)
                selection = paths.size()-1;
            else
                selection--;
            pressed = true;
            change = true;
        } else if(Frontend::getABtn()) {
            path = paths[selection];
            break;
        }
    }

    //reset screen rotation
    screen->TFTsetRotation(screen->TFT_rotate_e::TFT_Degrees_0);

    return path;
}

void draw_menu(ST7735_TFT* screen, std::vector<std::string>& paths, size_t selection) {
    screen->IMClear();

    size_t path_len;
    std::string filename;
    path_len = std::string(SCRIPT_PATH).length();

    //update offset of menu items
    menu_offset = selection * TEXT_PIXEL_HEIGHT;

    for(size_t i=0; i<paths.size(); i++) {
        //extract the name of the script from the path
        filename = paths[i].substr(path_len, paths[i].length()-path_len);

        //calculate if we are still rendering on screen (avoid overflow/underflow when calculating text position)
        uint8_t text_y = (SCREEN_HEIGHT/2)+((uint8_t)i*TEXT_PIXEL_HEIGHT) - (uint8_t)menu_offset - TEXT_PIXEL_HEIGHT;

        //make sure we're drawing on the screen (prevent underflow/overflow)
        if(text_y < 0 || text_y>=SCREEN_HEIGHT-TEXT_PIXEL_HEIGHT)
            continue;

        screen->TFTdrawText(TEXT_LEFT_BUFFER, text_y, 
                            (char*)filename.c_str(),
                            ST7735_WHITE, 
                            ST7735_BLACK, 
                            1, true); //draw text to the in memory buffer of the screen
    }

    //draw line under selected path
    uint8_t line_length = paths[selection].substr(path_len, paths[selection].length()-path_len).length() * 6;
    screen->TFTdrawFastHLine(TEXT_LEFT_BUFFER, (uint8_t)((SCREEN_HEIGHT/2)-(TEXT_PIXEL_HEIGHT/2)), line_length, 0xFFFF, true);

    screen->IMDisplay(); //render the in memory buffer to the physical screen
}

void readFiles(std::vector<std::string>& paths) {
    for (const auto & entry : fs::directory_iterator(SCRIPT_PATH))
        paths.push_back(entry.path());
}