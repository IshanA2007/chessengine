//
// Created by Ishan Ajwani on 8/8/26.
//

#ifndef CHESSENGINE_TYPES_H
#define CHESSENGINE_TYPES_H
#include <cstdint>

using Bitboard = uint64_t;

enum Piece {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum Colored_Piece {
    WP,
    WN,
    WB,
    WR,
    WQ,
    WK,
    BP,
    BN,
    BB,
    BR,
    BQ,
    BK
};



enum Color {
    WHITE,
    BLACK,
    BOTH
};

constexpr Colored_Piece make_piece(const Color c, const Piece p) {
    return static_cast<Colored_Piece>(p + 6 * c);
}

enum CastlingRights : uint8_t{
    WHITE_OO = 1, //kingside
    WHITE_OOO = 2, //queenside
    BLACK_OO = 4,
    BLACK_OOO = 8
};

// 0 -> a1, 63 -> h8
enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NO_SQUARE
};


#endif //CHESSENGINE_TYPES_H
