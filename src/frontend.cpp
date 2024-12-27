#include "frontend.hpp"
#include "screen.hpp"

using namespace Frontend;

#if BUILD_FOR_ARDUINO
    //arduino port for frontend
#else
    //PC port using opencv
    #include <opencv2/opencv.hpp>
    #include <opencv2/imgproc/imgproc.hpp>

    cv::Mat display = cv::Mat::zeros(cv::Size(SCREEN_WIDTH, SCREEN_HEIGHT), 0);

    void Frontend::drawScreen() {
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