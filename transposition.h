//
// Created by Ishan Ajwani on 8/11/26.
//

#ifndef CHESSENGINE_TRANSPOSITION_H
#define CHESSENGINE_TRANSPOSITION_H

#include "moves.h"

struct TTEntry {
    uint64_t key;
    Move best_move;
    int16_t score;
    uint8_t depth;
    uint8_t bound;
};

enum Bound : uint8_t {
    BOUND_NONE  = 0,   //empty slot
    BOUND_EXACT = 1,
    BOUND_LOWER = 2,
    BOUND_UPPER = 3
};

//modifies out_score and out_move
bool tt_probe(uint64_t hash, int depth, int alpha, int beta, int& out_score, Move& out_move);

void tt_store(uint64_t hash, int depth, int score, Move best_move, int alpha_original, int beta);

void tt_init();

void tt_clear();

#endif //CHESSENGINE_TRANSPOSITION_H
