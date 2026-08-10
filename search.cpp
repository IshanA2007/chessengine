//
// Created by Ishan Ajwani on 8/10/26.
//

#include "search.h"

#include <iostream>
#include <ostream>

#include "eval.h"
#include "movegen.h"

static uint64_t g_nodes = 0;
static std::chrono::steady_clock::time_point g_search_start;

static int64_t elapsed_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - g_search_start).count();
}


int minimax(Board& board, const int depth, int alpha, int beta) {
    g_nodes++;
    if (depth == 0) {
        return eval(board);
    }

    MoveList moves;
    generate_moves(board, moves);
    int legal = 0;
    int best_score = -INF_SCORE;

    for (int i = 0; i<moves.count; i++) {
        const Move m = moves.moves[i];
        Board board_copy = board;
        board_copy.make_move(m);
        if (!board_copy.last_move_was_legal()) {
            continue;
        }
        legal++;
        int move_score = -minimax(board_copy, depth - 1, -beta, -alpha);

        best_score = std::max(move_score, best_score);

        if (move_score >= beta) {
            break;
        }

        alpha = std::max(alpha, move_score);
    }

    if (!legal) {
        const bool in_check = board.is_square_attacked(board.king_square(), static_cast<Color>(!board.color_to_move));
        return in_check ? CHECKMATE_SCORE - depth : 0;
    }

    return best_score;
}

Move search_root(const Board& board, const int depth) {

    MoveList moves;
    generate_moves(board, moves);
    Move best_move = NO_MOVE;
    int alpha = -INF_SCORE;
    for (int i = 0; i < moves.count; i++) {
        const Move m = moves.moves[i];
        Board board_copy = board;
        board_copy.make_move(m);
        if (!board_copy.last_move_was_legal()) {
            continue;
        }
        int score = -minimax(board_copy, depth-1, -INF_SCORE, -alpha);
        if (score > alpha) {
            alpha = score;
            best_move = m;
        }
    }
    std::cout << "info depth " << depth << " nodes " << g_nodes << " score cp " << alpha << " pv " << move_to_string(best_move) << std::endl;
    return best_move;
}

Move search(const Board& board, const GoLimits& limits) {
    g_nodes = 0;
    g_search_start = std::chrono::steady_clock::now();

    int64_t budget_ms = -1;
    if (limits.movetime > 0) {
        budget_ms = limits.movetime - 30;               // margin safety
    } else {
        const int mytime = board.color_to_move == WHITE ? limits.wtime : limits.btime;
        const int myinc  = board.color_to_move == WHITE ? limits.winc  : limits.binc;
        if (mytime > 0)
            budget_ms = mytime / 25 + myinc / 2 - 30;
    }


    const int max_depth = limits.depth > 0 ? limits.depth : 6;
    Move best_move = NO_MOVE;
    std::cout << "info string budget " << budget_ms << " elapsed " << elapsed_ms() << std::endl;
    for (int d = 1; d <= max_depth; d++) {
        best_move = search_root(board, d);
        if (budget_ms > 0 && elapsed_ms() > budget_ms / 2) break;

    }
    return best_move;
}



