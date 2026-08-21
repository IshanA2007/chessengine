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
    const int score = (mg_score * mg_phase + eg_score * (24 - mg_phase)) / 24;
    return std::clamp(score, -MATE_BOUND + 1, MATE_BOUND - 1);
}

int mvv_score_of(const Board &board, const Move m) {

    if (is_promotion(m)) {
        int score = 100'000 + 10 * piece_values[promotion_of(m)];
        if (is_capture(m))
            score += piece_values[type_of(board.mailbox[to_square(m)])];
        return score;
    }
    if (!is_capture(m)) {
        return 0;
    }
    const int attacker = type_of(board.mailbox[from_square(m)]);
    const int victim = flags(m) == EN_PASSANT ? PAWN : type_of(board.mailbox[to_square(m)]);
    return 10 * piece_values[victim] - piece_values[attacker] + CAPTURE_BONUS;
}

int see(const Board &board, Move m) {
    const Square from_sq = from_square(m);
    const Square to_sq = to_square(m);
    const Piece victim = (flags(m) == EN_PASSANT) ? PAWN : type_of(board.mailbox[to_sq]);
    Piece attacker = type_of(board.mailbox[from_sq]);

    Bitboard occupancy = board.occupancy_bitboards[BOTH];

    occupancy ^= (1ULL << from_sq);

    Bitboard attackers = board.attackers_to(to_sq, occupancy);

    auto opp_color_to_move = static_cast<Color>(!board.color_to_move);

    int gain[32]{};

    gain[0] = piece_values[victim];

    int capture_depth = 0;

    const Bitboard diagonal_attackers = board.piece_bitboards[WB] | board.piece_bitboards[WQ] | board.piece_bitboards[BB] | board.piece_bitboards[BQ];
    const Bitboard line_attackers = board.piece_bitboards[WR] | board.piece_bitboards[WQ] | board.piece_bitboards[BR] | board.piece_bitboards[BQ];

    while (true) {
        //find opponents cheapest attacker
        Piece piece = {};
        Bitboard piece_bb = 0;
        for (const Piece pt :  {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING}) {
            piece_bb = board.piece_bitboards[make_piece(opp_color_to_move, pt)] & attackers & occupancy;
            if (piece_bb) {
                piece = pt;
                break;
            }
        }
        if (!piece_bb) {
            break;
        }
        if (piece == KING) {
            const Bitboard enemies_left_bb = occupancy & attackers & board.occupancy_bitboards[!opp_color_to_move];
            if (enemies_left_bb) {
                break;
            }
        }

        capture_depth += 1;
        gain[capture_depth] = piece_values[attacker] - gain[capture_depth-1];

        occupancy ^= piece_bb & -piece_bb;

        attackers |= rook_attacks(to_sq, occupancy) & line_attackers;
        attackers |= bishop_attacks(to_sq, occupancy) & diagonal_attackers;

        attacker = piece;
        opp_color_to_move = static_cast<Color>(!opp_color_to_move);
    }

    while (capture_depth > 0) {
        gain[capture_depth-1] = -std::max(gain[capture_depth], -gain[capture_depth-1]);
        capture_depth--;
    }

    return gain[0];
}
