#include <string>

#include "frontend.hpp"
#include "screen.hpp"

using namespace Frontend;

void throwFrontendError(std::string message);

#if BUILD_FOR_RASPI
    //arduino port for frontend
    #include <bcm2835.h> 
    #include "ST7735_TFT.hpp"

    //code taken from Display_Lib_RPI GitHub: https://github.com/gavinlyonsrepo/Display_Lib_RPI/blob/main/examples/st7735/Hello_world_SWSPI/main.cpp
    ST7735_TFT myTFT(SCREEN_HEIGHT, SCREEN_WIDTH);

    uint8_t SetupSWSPI(void); // setup + user options for software SPI

    void Frontend::init() {
        if(SetupSWSPI()!=0)
            throwFrontendError("Unable to initialize SPI screen!");

        myTFT.TFTfillScreen(ST7735_BLACK);
        myTFT.IMClear();
    }

    void Frontend::cleanUp() {
        myTFT.TFTfillScreen(ST7735_BLACK);
        myTFT.TFTPowerDown();
        bcm2835_close();
    }

    void Frontend::drawScreen() {
        uint16_t color;
        for(int x=0; x<SCREEN_WIDTH; x++) {
            for(int y=0; y<SCREEN_HEIGHT; y++) {
                color = myTFT.Color565(((int16_t)screen.screenBuff[y][x][0]), ((int16_t)screen.screenBuff[y][x][1]), ((int16_t)screen.screenBuff[y][x][2]));
                myTFT.IMDrawPixel(y, x, color);
            }
        }

        myTFT.IMDisplay();
    }

    float Frontend::getHorAxis() {
        return 0.0;
    }

    float Frontend::getVertAxis() {
        return 0.0;
    }

    bool Frontend::getABtn() {
        return false;
    }

    bool Frontend::getBBtn() {
        return false;
    }

    bool Frontend::getExitBtn() {
        return false;
    }

    uint8_t SetupSWSPI(void)
    {
        if(!bcm2835_init())
        {
            std::cout << "Error 1201 Problem with init bcm2835 library" << std::endl;
            return 2;
        }

        // GPIO
        int8_t RST_TFT  = 25;
        int8_t DC_TFT   = 24;
        int8_t SCLK_TFT = 6;
        int8_t SDIN_TFT = 5;
        int8_t CS_TFT   = 21;

        uint8_t OFFSET_COL = 0;  // 2, These offsets can be adjusted for any issues->
        uint8_t OFFSET_ROW = 0; // 3, with manufacture tolerance/defects at edge of display
        uint16_t TFT_WIDTH = SCREEN_HEIGHT;// Screen width in pixels (128)
        uint16_t TFT_HEIGHT = SCREEN_WIDTH; // Screen height in pixels (160)

        uint16_t SWSPI_CommDelay = 0; //uS GPIO SW SPI delay

        // ** USER OPTION 1 GPIO/SPI TYPE SW **
        myTFT.TFTSetupGPIO(RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);
        //********************************************
        // ** USER OPTION 2 Screen Setup **
        myTFT.TFTInitScreenSize(OFFSET_COL, OFFSET_ROW , TFT_WIDTH , TFT_HEIGHT);
        // ***********************************
        // ** USER OPTION 3 PCB_TYPE **
        // pass enum to param1 ,4 choices,see README
        if(myTFT.TFTInitPCBType(myTFT.TFT_ST7735R_Red, SWSPI_CommDelay) != 0)
        {
            bcm2835_close();
            return 3;
        }
        //*****************************

        TFT_MILLISEC_DELAY(50);
        return 0;
    }
#else
    //PC port using opencv
    #include <opencv2/opencv.hpp>
    #include <opencv2/imgproc/imgproc.hpp>
    #include <opencv2/core/utils/logger.hpp>

    void Frontend::init() {}
    void Frontend::cleanUp() {}

    void Frontend::drawScreen() {
        //convert virtual screen to opencv Mat
        cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

        cv::Mat display(SCREEN_HEIGHT, SCREEN_WIDTH, CV_8UC3, screen.screenBuff);
        cv::cvtColor(display, display, cv::COLOR_RGB2BGR);
        
        //draw Mat in window
        cv::imshow("Squiggly Project", display);
        cv::waitKey(SCREEN_REFRESH_DELAY);
    }

    //getting keboard input
    #if _WIN32
        #include <windows.h>

        float Frontend::getHorAxis() {
            if(GetAsyncKeyState(VK_RIGHT) & 0x8000)
                return 1.0;
            else if(GetAsyncKeyState(VK_LEFT) & 0x8000)
                return -1.0;
            else
                return 0.0;
        }

        float Frontend::getVertAxis() {
            if(GetAsyncKeyState(VK_UP) & 0x8000)
                return 1.0;
            else if(GetAsyncKeyState(VK_DOWN) & 0x8000)
                return -1.0;
            else
                return 0.0;
        }

        bool Frontend::getABtn() {
            return GetAsyncKeyState(WIN_A_BTN_CODE) & 0x8000;
        }

        bool Frontend::getBBtn() {
            return GetAsyncKeyState(WIN_B_BTN_CODE) & 0x8000;
        }

        bool Frontend::getExitBtn() {
            return GetAsyncKeyState(VK_ESCAPE) & 0x8000;
        }
    #else //todo: implement keyboard input for mac and linux
        float Frontend::getHorAxis() {
            return 0.0;
        }

        float Frontend::getVertAxis() {
            return 0.0;
        }

        bool Frontend::getABtn() {
            return false;
        }

        bool Frontend::getBBtn() {
            return false;
        }

        bool Frontend::getExitBtn() {
            return false;
        }
    #endif
#endif

void throwFrontendError(std::string message) {
    throw std::runtime_error("Program frontend failed! : " + message);
}