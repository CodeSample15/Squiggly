#include "frontend.hpp"

#if BUILD_FOR_ARDUINO
    //arduino port for frontend
#else
    //PC port using opencv
    #include <opencv2/opencv.hpp>

    
#endif