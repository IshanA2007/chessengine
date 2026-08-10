//
// Created by Ishan Ajwani on 8/10/26.
//

#include "eval.h"

int eval(Board &board) {
    static constexpr int piece_values[6] = {
        100,   // PAWN
        320,   // KNIGHT
        330,   // BISHOP
        500,   // ROOK
        900,   // QUEEN
        0      // KING — not counted; game ends before material matters
    };

    int white_score = 0;
    int black_score = 0;

    for (int piece = WP; piece <= WK; piece++) {
        const int count = std::popcount(board.piece_bitboards[piece]);
        white_score += count * piece_values[type_of(static_cast<Colored_Piece>(piece))];
    }

    for (int piece = BP; piece <= BK; piece++) {
        const int count = std::popcount(board.piece_bitboards[piece]);
        black_score += count * piece_values[type_of(static_cast<Colored_Piece>(piece))];
    }

    return board.color_to_move == WHITE
        ? white_score - black_score
        : black_score - white_score;

}
