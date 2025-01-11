#include "frontend.hpp"
#include "screen.hpp"

using namespace Frontend;

#if BUILD_FOR_ARDUINO
    //arduino port for frontend
#else
    //PC port using opencv
    #include <opencv2/opencv.hpp>
    #include <opencv2/imgproc/imgproc.hpp>
    #include <opencv2/core/utils/logger.hpp>

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
    #endif
#endif