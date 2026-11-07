cmake_minimum_required(VERSION 3.10)
project(auto_aim_minimal)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

add_executable(auto_aim_minimal
    src/main.cpp
    src/auto_aim/light_bar_detector.cpp

)

target_link_libraries(auto_aim_minimal ${OpenCV_LIBS})
target_link_libraries(auto_aim_minimal ${OpenCV_LIBS} opencv_geometry)