cmake_minimum_required(VERSION 3.18)

PROJECT (squiggly)

add_compile_options(-std=c++20 -Wall -Werror)

find_package( OpenCV REQUIRED )

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

include_directories( include ${OpenCV_INCLUDE_DIRS} )
file( GLOB SOURCES "src/*.cpp" "src/Built-In/*.cpp" )

add_executable( squiggly ${SOURCES} )
target_link_libraries( squiggly ${OpenCV_LIBS} )

install( TARGETS squiggly )