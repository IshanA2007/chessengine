cmake_minimum_required(VERSION 4.3)
project(chessengine)

set(CMAKE_CXX_STANDARD 20)

add_executable(chessengine main.cpp
        types.h
        board.cpp
        board.h
        attacks.h
        bitboard.h
        attacks.cpp
        moves.h
        movegen.cpp
        movegen.h
        uci.cpp
        uci.h
        search.cpp
        search.h
        eval.cpp
        eval.h
        zobrist.cpp
        zobrist.h)
