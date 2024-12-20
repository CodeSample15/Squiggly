cmake_minimum_required(VERSION 3.22)

set(OpenCV_DIR "C:/opencv/build/x64/vc16/lib") #change this to your local OpenCV directory

add_compile_options(-Wall -Werror -Wa,-mbig-obj)

PROJECT (squiggly)
find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( include )
file( GLOB SOURCES "src/*.cpp" )

add_executable( squiggly ${SOURCES} )
target_link_libraries( squiggly ${OpenCV_LIBS} )