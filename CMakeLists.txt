cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME BMP)
add_compile_options(-Wall)

project(${PROJECT_NAME} VERSION 1.0.0)

set(SOURCE_CODE
    ./bmp/bmp_file.hpp
    ./bmp/bmp_file.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_CODE} ./main.cpp)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME}
)
