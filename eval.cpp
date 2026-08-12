//
// Created by Ishan Ajwani on 8/10/26.
//

#include "eval.h"

int eval(const Board &board) {

    int middlegaminess[2] = {0, 0};
    int endgaminess[2] = {0, 0};
    int game_phase = 0;

    for (int piece = WP; piece <= BK; piece++) {
        const Color c = color_of(static_cast<Colored_Piece>(piece));
        const Piece p = type_of(static_cast<Colored_Piece>(piece));

        Bitboard piece_bb = board.piece_bitboards[piece];
        while (piece_bb) {
            const int square  = std::countr_zero(piece_bb);
            piece_bb &= piece_bb - 1;
            const int pst_idx = (c == WHITE) ? (square ^ 56) : square;
            middlegaminess[c] += mg_value[p] + mg_table[p][pst_idx];
            endgaminess[c] += eg_value[p] + eg_table[p][pst_idx];
            game_phase += game_phase_inc[p];
        }
    }

    const Color us = board.color_to_move;
    const int mg_score = middlegaminess[us] - middlegaminess[!us];
    const int eg_score = endgaminess[us] - endgaminess[!us];
    const int mg_phase = std::min(game_phase, 24);
    return (mg_score * mg_phase + eg_score * (24 - mg_phase)) / 24;
}

int score_of(const Board &board, const Move m) {
    if (!is_capture(m)) {
        return 0;
    }
    const int attacker = type_of(board.mailbox[from_square(m)]);
    const int victim = flags(m) == EN_PASSANT ? PAWN : type_of(board.mailbox[to_square(m)]);
    return 10 * piece_values[victim] - piece_values[attacker] + 100000;
}
