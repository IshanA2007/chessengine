//
// Created by Ishan Ajwani on 8/10/26.
//

#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include "uci.h"
#include "board.h"

int quiescence(Board& board, int alpha, int beta);

Move search(const Board& board, const GoLimits &limits);

Move search_root(Board& board, int depth);

void history_reset(uint64_t root_hash);

void history_push(uint64_t h);

#endif //CHESSENGINE_SEARCH_H
