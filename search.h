//
// Created by Ishan Ajwani on 8/10/26.
//

#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include "uci.h"
#include "board.h"

int quiescence(const Board& board, int alpha, int beta);

Move search(const Board& board, const GoLimits &limits);

int minimax(const Board& board, int depth, int alpha, int beta);

Move search_root(const Board& board, int depth);

void history_reset(uint64_t root_hash);

void history_push(uint64_t h);

#endif //CHESSENGINE_SEARCH_H
