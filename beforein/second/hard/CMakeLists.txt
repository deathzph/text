cmake_minimum_required(VERSION 3.16)
project(img_basic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_executable(img_basic src/main.cpp)

target_include_directories(img_basic SYSTEM PRIVATE third_party)

if(MSVC)
    target_compile_options(img_basic PRIVATE /W4 /utf-8)
else()
    target_compile_options(img_basic PRIVATE -Wall -Wextra)
endif()

add_custom_command(TARGET img_basic POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data
            $<TARGET_FILE_DIR:img_basic>/data
    COMMENT "复制测试图片到输出目录")

message(STATUS "用法: img_basic [输入图片] [输出目录] [阈值]")