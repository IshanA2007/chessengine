//
// Created by Ishan Ajwani on 8/9/26.
//

#ifndef CHESSENGINE_UCI_H
#define CHESSENGINE_UCI_H
#include <string>

#include "moves.h"
#include "board.h"

struct GoLimits { int depth = -1; int movetime = -1;
    int wtime = -1, btime = -1, winc = 0, binc = 0; bool infinite = false; };

Move parse_move(const Board& board, const std::string &token);

void handle_position(Board& board, std::istringstream& ss);

void handle_go(const Board& board, std::istringstream& ss);

#endif //CHESSENGINE_UCI_H
