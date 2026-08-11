//
// Created by Ishan Ajwani on 8/10/26.
//

#include "eval.h"

int eval(const Board &board) {

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

int score_of(const Board &board, const Move m) {
    if (!is_capture(m)) {
        return 0;
    }
    const int attacker = type_of(board.mailbox[from_square(m)]);
    const int victim = flags(m) == EN_PASSANT ? PAWN : type_of(board.mailbox[to_square(m)]);
    return 10 * piece_values[victim] - piece_values[attacker] + 100000;
}
