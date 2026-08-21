//
// Created by Ishan Ajwani on 8/10/26.
//

#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include "uci.h"
#include "board.h"

int quiescence(Board& board, int alpha, int beta, int ply);

Move search(const Board& board, const GoLimits &limits);

bool search_root(Board& board, int depth, Move& out_best);

void history_reset(uint64_t root_hash);

void history_push(uint64_t h);

uint64_t nodes_searched();

#endif //CHESSENGINE_SEARCH_H
