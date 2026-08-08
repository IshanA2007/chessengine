cmake_minimum_required(VERSION 4.3)
project(chessengine)

set(CMAKE_CXX_STANDARD 20)

add_executable(chessengine main.cpp
        types.h
        board.cpp
        board.h)
