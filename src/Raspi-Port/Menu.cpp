#include <stdlib.h>
#include "Raspi-Port/Menu.hpp"
#include "screen.hpp"

void readFiles(std::vector<std::string>& paths);
void draw_menu(ST7735_TFT* screen, std::vector<std::string>& paths, size_t selection);

float menu_offset = 0;

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
    size_t selection = 0;
    bool selection_made = false;
    std::string path = "";

    bool pressed = false;
    while(!selection_made) {
        draw_menu(screen, paths, selection);
        do {
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
            if(selection<1)
                selection = paths.size()-1;
            else
                selection--;
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

void draw_menu(ST7735_TFT* screen, std::vector<std::string>& paths, size_t selection) {
    screen->IMClear();

    size_t path_len;
    std::string filename;

    //update offset
    menu_offset = selection * TEXT_PIXEL_HEIGHT;

    for(size_t i=0; i<paths.size(); i++) {
        //extract the name of the script from the path
        path_len = std::string(SCRIPT_PATH).length();
        filename = paths[i].substr(path_len, paths[i].length()-path_len);

        //calculate if we are still rendering on screen (avoid overflow/underflow when calculating text position)
        uint8_t text_y = (SCREEN_HEIGHT/2)+((uint8_t)i*TEXT_PIXEL_HEIGHT) - (uint8_t)menu_offset - TEXT_PIXEL_HEIGHT;

        screen->TFTdrawText(TEXT_LEFT_BUFFER, text_y, 
                            (char*)filename.c_str(),
                            ST7735_WHITE, 
                            ST7735_BLACK, 
                            1, true); //draw text to the in memory buffer of the screen
    }

    //todo: draw line under selected path
    //screen->TFTdrawFastHLine(TEXT_LEFT_BUFFER, ((uint8_t)i*TEXT_PIXEL_HEIGHT) - (uint8_t)menu_offset + (uint8_t)(TEXT_PIXEL_HEIGHT/2), 2, 0xFFFF);

    screen->IMDisplay(); //render the in memory buffer to the physical screen
}

void readFiles(std::vector<std::string>& paths) {
    paths.push_back(SCRIPT_PATH + "test");
    paths.push_back(SCRIPT_PATH + "another test");
    paths.push_back(SCRIPT_PATH + "Menu test");
    paths.push_back(SCRIPT_PATH + "Menu Item");
}