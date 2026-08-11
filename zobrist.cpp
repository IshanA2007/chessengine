//
// Created by Ishan Ajwani on 8/10/26.
//

#include "zobrist.h"

#include <random>

#include "board.h"

uint64_t zobrist_pieces[12][64];
uint64_t zobrist_castling[16];
uint64_t zobrist_ep_file[8];
uint64_t zobrist_side;

void init_zobrist() {
    std::mt19937_64 rng(0xabcdef1234567ULL);

    for (int pc = 0; pc < 12; pc++)
        for (int sq = 0; sq < 64; sq++)
            zobrist_pieces[pc][sq] = rng();

    for (int i = 0; i < 16; i++) zobrist_castling[i] = rng();
    for (int f = 0; f < 8;  f++) zobrist_ep_file[f]  = rng();
    zobrist_side = rng();
}
