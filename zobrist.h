//
// Created by Ishan Ajwani on 8/10/26.
//

#ifndef CHESSENGINE_ZOBRIST_H
#define CHESSENGINE_ZOBRIST_H
#include <cstdint>

extern uint64_t zobrist_pieces[12][64];
extern uint64_t zobrist_side;
extern uint64_t zobrist_castling[16];
extern uint64_t zobrist_ep_file[8];

void init_zobrist();


#endif //CHESSENGINE_ZOBRIST_H
