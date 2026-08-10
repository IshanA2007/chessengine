//
// Created by Ishan Ajwani on 8/8/26.
//

#ifndef CHESSENGINE_BITBOARD_H
#define CHESSENGINE_BITBOARD_H
#include <iostream>

inline void print_bitboard(Bitboard bb) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            const int sq = rank * 8 + file;
            std::cout << (bb & (1ULL << sq) ? '1' : '.');
            if (file < 7) std::cout << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

inline Bitboard shift_signed(const Bitboard bb, const int n) {
    return n >= 0 ? bb << n : bb >> -n;
}

constexpr Bitboard FILE_A = 0x0101010101010101ULL;
constexpr Bitboard FILE_B = 0x0202020202020202ULL;
constexpr Bitboard FILE_C = 0x0404040404040404ULL;
constexpr Bitboard FILE_D = 0x0808080808080808ULL;
constexpr Bitboard FILE_E = 0x1010101010101010ULL;
constexpr Bitboard FILE_F = 0x2020202020202020ULL;
constexpr Bitboard FILE_G = 0x4040404040404040ULL;
constexpr Bitboard FILE_H = 0x8080808080808080ULL;

constexpr Bitboard RANK_1 = 0x00000000000000FFULL;
constexpr Bitboard RANK_2 = 0x000000000000FF00ULL;
constexpr Bitboard RANK_3 = 0x0000000000FF0000ULL;
constexpr Bitboard RANK_4 = 0x00000000FF000000ULL;
constexpr Bitboard RANK_5 = 0x000000FF00000000ULL;
constexpr Bitboard RANK_6 = 0x0000FF0000000000ULL;
constexpr Bitboard RANK_7 = 0x00FF000000000000ULL;
constexpr Bitboard RANK_8 = 0xFF00000000000000ULL;

constexpr Bitboard WHITE_OO_EMPTY  = 0x0000000000000060ULL; // f1, g1
constexpr Bitboard WHITE_OOO_EMPTY = 0x000000000000000EULL; // b1, c1, d1
constexpr Bitboard BLACK_OO_EMPTY  = 0x6000000000000000ULL; // f8, g8
constexpr Bitboard BLACK_OOO_EMPTY = 0x0E00000000000000ULL; // b8, c8, d8

#endif //CHESSENGINE_BITBOARD_H
