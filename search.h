//
// Created by Ishan Ajwani on 8/10/26.
//

#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include "uci.h"
#include "board.h"

Move search(const Board& board, const GoLimits &limits);

int minimax(Board &board, int depth);

Move search_root(const Board& board, int depth);

#endif //CHESSENGINE_SEARCH_H
