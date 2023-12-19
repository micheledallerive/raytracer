cmake_minimum_required(VERSION 3.15)

project(Raytracer)

#set(CMAKE_CXX_COMPILER clang++-14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wpedantic -Ofast -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories("include")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# when executing Raytracer, convert the image .ppm to .png
add_executable(Raytracer ${SOURCES} main.cpp
        src/raytracer.cpp)
#add_custom_command(TARGET Raytracer POST_BUILD
#        COMMAND convert ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/result.ppm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/result.png)

