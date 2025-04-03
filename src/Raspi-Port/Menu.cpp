#include "Raspi-Port/Menu.hpp"

void readFiles(std::vector<std::string>& paths);

void run_menu(std::vector<std::string>& fileLines) {
    //start screen and get screen pointer
    Frontend::init();
    ST7735_TFT* screen = Frontend::get_tft();
    screen->IMClear();
    screen->TFTFontNum(screen->TFTFont_Default);

    //fetch paths from file system
    std::vector<std::string> paths;
    readFiles(paths);

    for(size_t i=0; i<paths.size(); i++) {
        screen->TFTdrawText(5, i*TEXT_PIXEL_HEIGHT, paths[i].c_str(), ST7735_WHITE, ST7735_BLACK, 1, true); //draw text to the in memory buffer of the screen
    }

    screen->IMDisplay(); //render the in memory buffer to the physical screen
    TFT_MILLISEC_DELAY(3000);
}

void readFiles(std::vector<std::string>& paths) {
    paths.push_back("test");
    paths.push_back("another test");
    paths.push_back("Menu test");
    paths.push_back("Menu Item");
}