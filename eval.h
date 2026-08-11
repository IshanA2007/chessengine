//
// Created by Ishan Ajwani on 8/10/26.
//

#ifndef CHESSENGINE_EVAL_H
#define CHESSENGINE_EVAL_H
#include "board.h"

constexpr int INF_SCORE = 1'000'000'000;
constexpr int CHECKMATE_SCORE = -100'000;

static constexpr int piece_values[6] = {
    100,   // PAWN
    320,   // KNIGHT
    330,   // BISHOP
    500,   // ROOK
    900,   // QUEEN
    0      // KING
};

int eval(const Board& board);

int score_of(const Board& board, Move m);
#endif //CHESSENGINE_EVAL_H
