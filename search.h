//
// Created by Ishan Ajwani on 8/10/26.
//

#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include "uci.h"
#include "board.h"

Move search(const Board& board, GoLimits limits);

#endif //CHESSENGINE_SEARCH_H
