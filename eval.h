//
// Created by Ishan Ajwani on 8/10/26.
//

#ifndef CHESSENGINE_EVAL_H
#define CHESSENGINE_EVAL_H
#include "board.h"

constexpr int INF_SCORE = 1'000'000'000;

int eval(Board& board);

#endif //CHESSENGINE_EVAL_H
