//
// Created by Ishan Ajwani on 8/9/26.
//

#ifndef CHESSENGINE_MOVES_H
#define CHESSENGINE_MOVES_H

#include <cstdint>

#include "types.h"
using Move = uint16_t;

inline Move make_move(const Square from, const Square to, const int flags) {
    return static_cast<Move>(from | (to << 6) | (flags << 12));
}

inline Square from_square(const Move m) {
    return static_cast<Square>(m & 0b0000000000111111);
}

inline Square to_square(const Move m) {
    return static_cast<Square>((m & 0b0000111111000000) >> 6);
}

inline int flags(const Move m) {
    return (m & 0b1111000000000000) >> 12;
}

struct MoveList {
    Move moves[256]{};
    int count = 0;
};

enum MoveFlag : int {
    QUIET            = 0b0000,
    DOUBLE_PUSH      = 0b0001,
    CASTLE_KINGSIDE  = 0b0010,
    CASTLE_QUEENSIDE = 0b0011,
    CAPTURE          = 0b0100,
    EN_PASSANT       = 0b0101,
    PROMO_N          = 0b1000,
    PROMO_B          = 0b1001,
    PROMO_R          = 0b1010,
    PROMO_Q          = 0b1011,
    PROMO_N_CAPTURE  = 0b1100,
    PROMO_B_CAPTURE  = 0b1101,
    PROMO_R_CAPTURE  = 0b1110,
    PROMO_Q_CAPTURE  = 0b1111,
};

constexpr Move NO_MOVE = 0;

constexpr bool is_capture(const Move m) {
    return (flags(m) & CAPTURE);
}

constexpr bool is_promotion(const Move m) {
    return (flags(m) & 0b1000);
}

//assumes move is a promotion: does'nt check
constexpr Piece promotion_of(const Move m) {
    return static_cast<Piece>((flags(m) & 0b0011) + KNIGHT);
}

inline std::string move_to_string(const Move m) {
    static constexpr char file_chars[] = "abcdefgh";
    static constexpr char promo_chars[] = "nbrq";

    const Square from = from_square(m);
    const Square to = to_square(m);

    std::string s;
    s += file_chars[file_of(from)];
    s += static_cast<char>('1' + rank_of(from));
    s += file_chars[file_of(to)];
    s += static_cast<char>('1' + rank_of(to));
    if (is_promotion(m))
        s += promo_chars[flags(m) & 0b0011];
    return s;
}


#endif //CHESSENGINE_MOVES_H
