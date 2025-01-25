cmake_minimum_required(VERSION 3.18)

PROJECT (squiggly)

add_compile_options(-std=c++20 -Wall -Werror)
file( GLOB SOURCES "src/*.cpp" "src/Built-In/*.cpp" )

if(false)
    #regular build (pc)
    find_package( OpenCV REQUIRED )

    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

    include_directories( include ${OpenCV_INCLUDE_DIRS} )

    add_executable( squiggly ${SOURCES} )
    target_link_libraries( squiggly ${OpenCV_LIBS} )
else()
    #console build (raspberry pi)
    find_package( WiringPi REQUIRED )

    message(STATUS "WiringPi version: ${WiringPi_VERSION}")
    message(STATUS "WiringPi include directories: ${WiringPi_INCLUDE_DIRS}")
    message(STATUS "WiringPi libraries: ${WiringPi_LIBS}")

    include_directories( include ${WiringPi_INCLUDE_DIRS} )

    add_executable( squiggly ${SOURCES} )
    target_link_libraries( squiggly ${WiringPi_LIBS} -lbcm2835 -lrt -lST7735_TFT_RPI )
endif()

install( TARGETS squiggly )