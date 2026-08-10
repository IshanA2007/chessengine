//
// Created by Ishan Ajwani on 8/10/26.
//

#include "search.h"

#include <iostream>
#include <ostream>
#include <random>

#include "eval.h"
#include "movegen.h"

Move search_root(const Board& board, const int depth) {
    MoveList moves;
    generate_moves(board, moves);
    Move best_move = NO_MOVE;
    int best_score = -INF_SCORE;
    for (int i = 0; i < moves.count; i++) {
        const Move m = moves.moves[i];
        Board board_copy = board;
        board_copy.make_move(m);
        const auto king_square = static_cast<Square>(std::countr_zero(board_copy.piece_bitboards[make_piece(board.color_to_move, KING)]));
        if (board_copy.is_square_attacked(king_square, board_copy.color_to_move)) {
            continue;
        }
        int score = -minimax(board_copy, depth-1);
        if (score > best_score) {
            best_score = score;
            best_move = m;
        }
    }
    std::cout << "info depth " << depth << " score cp " << best_score << " pv " << move_to_string(best_move) << std::endl;
    return best_move;
}

Move search(const Board& board, const GoLimits& limits) {
    const int max_depth = limits.depth > 0 ? limits.depth : 6;
    Move best_move = NO_MOVE;
    for (int d = 1; d <= max_depth; d++) {
        best_move = search_root(board, d);

    }
    return best_move;
}


int minimax(Board& board, const int depth) {
    if (depth == 0) {
        return eval(board);
    }

    int best_score = -INF_SCORE;

    MoveList moves;
    generate_moves(board, moves);
    int legal = 0;

    for (int i = 0; i<moves.count; i++) {
        const Move m = moves.moves[i];
        Board board_copy = board;
        board_copy.make_move(m);
        const auto king_square = static_cast<Square>(std::countr_zero(board_copy.piece_bitboards[make_piece(board.color_to_move, KING)]));
        if (board_copy.is_square_attacked(king_square, board_copy.color_to_move)) {
            continue;
        }
        legal++;
        best_score = std::max(best_score, -minimax(board_copy, depth - 1));
    }

    if (!legal) {
        const auto king_square = static_cast<Square>(std::countr_zero(board.piece_bitboards[make_piece(board.color_to_move, KING)]));
        const bool in_check = board.is_square_attacked(king_square, static_cast<Color>(!board.color_to_move));
        return in_check ? -100000000 - depth : 0;
    }

    return best_score;
}
