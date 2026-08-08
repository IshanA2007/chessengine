//
// Created by Ishan Ajwani on 8/8/26.
//

#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H
#include <string>

#include "types.h"

using namespace std;

struct Board {
    Bitboard piece_bitboards[12];
    Bitboard occupancy_bitboards[3];

    Color side_to_move;

    // smallest 4 bits matter: abcd -> cd rep white castling, ab rep black castling
    uint8_t castling_rights;

    Square en_passant_square;

    uint8_t fifty_clock;

    // necessary for FEN extraction
    uint16_t fullmove_number;

    static void set_from_fen(string fen);

    static void print();
};


#endif //CHESSENGINE_BOARD_H
