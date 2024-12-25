#include "frontend.hpp"
#include "screen.hpp"

using namespace Frontend;

#if BUILD_FOR_ARDUINO
    //arduino port for frontend
#else
    //PC port using opencv
    #include <opencv2/opencv.hpp>
    #include <opencv2/imgproc/imgproc.hpp>

    using namespace cv;

    Mat display = Mat::zeros(Size(SCREEN_WIDTH, SCREEN_HEIGHT), 0);

    void Frontend::drawScreen() {
        imshow("Squiggly Project", display);
    }
#endif