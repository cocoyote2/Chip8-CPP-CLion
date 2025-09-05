cmake_minimum_required(VERSION 3.21)
project(Chip8_CPP_CLion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)

add_executable(Chip8_CPP_CLion
        main.cpp
        Chip8.cpp
        SDLLayer.cpp
)

target_link_libraries(Chip8_CPP_CLion PRIVATE SDL3::SDL3)