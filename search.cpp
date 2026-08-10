//
// Created by Ishan Ajwani on 8/10/26.
//

#include "search.h"

#include <random>

#include "movegen.h"

static std::mt19937 rng(std::random_device{}());

Move search(const Board& board, GoLimits limits) {
    MoveList moves;
    generate_moves(board, moves);
    MoveList legal_moves;
    for (int i = 0; i < moves.count; i++) {
        const Move m = moves.moves[i];
        Board board_copy = board;
        board_copy.make_move(m);
        const auto king_square = static_cast<Square>(std::countr_zero(board_copy.piece_bitboards[make_piece(board.color_to_move, KING)]));
        if (board_copy.is_square_attacked(king_square, board_copy.color_to_move)) {
            continue;
        }
        legal_moves.moves[legal_moves.count++] = m;
    }
    if (legal_moves.count == 0) {
        return NO_MOVE;
    }
    std::uniform_int_distribution<int> dist(0, legal_moves.count - 1);
    return legal_moves.moves[dist(rng)];
}
