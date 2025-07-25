cmake_minimum_required(VERSION 3.28)

PROJECT (squiggly LANGUAGES CXX)

add_compile_options(-Wall -Werror) #use this for development, GET RID OF ALL WARNINGS BEFORE PUSHING
#add_compile_options(-Wall)
file( GLOB SOURCES "src/*.cpp" "src/Built-In/*.cpp" )

if(true)
    #regular build (pc)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    #include SFML to project
    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.1
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
    FetchContent_MakeAvailable(SFML)

    include_directories( include )

    add_executable( squiggly ${SOURCES} )
    target_compile_features( squiggly PRIVATE cxx_std_17 )
    target_link_libraries( squiggly PRIVATE SFML::Graphics )
else()
    #console build (raspberry pi)
    include_directories( include lib/joystick )

    file( GLOB RASPI_SRC "src/Raspi-Port/*.cpp")
    add_executable( squiggly ${SOURCES} ${RASPI_SRC} "lib/joystick/joystick.cc")
    target_link_libraries( squiggly -lbcm2835 -lrt -lST7735_TFT_RPI )
endif()

install( TARGETS squiggly )