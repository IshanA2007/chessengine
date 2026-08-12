//
// Created by Ishan Ajwani on 8/10/26.
//

#include "search.h"

#include <iostream>
#include <ostream>

#include "eval.h"
#include "movegen.h"
#include "transposition.h"

static uint64_t g_nodes = 0;
static uint64_t g_history[1024];
static int g_hist_count = 0;
static std::chrono::steady_clock::time_point g_search_start;

static int64_t elapsed_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - g_search_start).count();
}

int quiescence(const Board &board, int alpha, const int beta) {
    g_nodes++;
    const int cur_eval = eval(board);

    if (cur_eval >= beta) {
        return cur_eval;
    }

    if (cur_eval > alpha) {
        alpha = cur_eval;
    }

    MoveList moves;
    generate_captures(board, moves);
    //MVV-LVA
    int scores[256];
    for (int i = 0; i < moves.count; i++)
        scores[i] = score_of(board, moves.moves[i]);

    for (int i = 0; i < moves.count; i++) {
        //selection sort best move into i
        int best = i;
        for (int j = i+1; j < moves.count; j++) {
            if (scores[j] > scores[best]) {
                best = j;
            }
        }
        std::swap(moves.moves[i], moves.moves[best]);
        std::swap(scores[i], scores[best]);


        const Move m = moves.moves[i];
        Board board_copy = board;
        board_copy.make_move(m);
        if (!board_copy.last_move_was_legal()) {
            continue;
        }
        int score = -quiescence(board_copy, -beta, -alpha);
        if (score >= beta) {
            return score;
        }
        if (score > alpha) {
            alpha = score;
        }
    }
    return alpha;


}


int minimax(const Board& board, const int depth, int alpha, const int beta, const int ply) {
    g_nodes++;

    if (board.fifty_clock >= 100) {
        return 0;
    }
    for (int k = g_hist_count - 3; k >= std::max(0, g_hist_count - 1 - board.fifty_clock); k-=2) {
        if (g_history[k] == board.hash) {
            return 0; //draw
        }
    }

    const int alpha_original = alpha;
    Move tt_move = NO_MOVE;
    int tt_score;

    if (tt_probe(board.hash, depth, alpha_original, beta, tt_score, tt_move)) {
        return tt_score;
    }
    if (depth == 0) {
        return quiescence(board, alpha, beta);
    }

    MoveList moves;
    generate_moves(board, moves);
    int legal = 0;
    int best_score = -INF_SCORE;
    Move best_move = NO_MOVE;

    //MVV LVA
    int scores[256];
    for (int i = 0; i < moves.count; i++)
        scores[i] = score_of(board, moves.moves[i]);

    if (tt_move) {
        for (int i = 0; i<moves.count; i++) {
            if (moves.moves[i] == tt_move) {
                scores[i] = TT_BONUS_SCORE;
                break;
            }
        }
    }

    for (int i = 0; i<moves.count; i++) {

        //selection sort best move into i
        int best = i;
        for (int j = i+1; j < moves.count; j++) {
            if (scores[j] > scores[best]) {
                best = j;
            }
        }
        std::swap(moves.moves[i], moves.moves[best]);
        std::swap(scores[i], scores[best]);


        const Move m = moves.moves[i];
        Board board_copy = board;
        board_copy.make_move(m);
        if (!board_copy.last_move_was_legal()) {
            continue;
        }
        legal++;
        g_history[g_hist_count++] = board_copy.hash;
        int move_score = -minimax(board_copy, depth - 1, -beta, -alpha, ply+1);
        g_hist_count--;
        if (move_score > best_score) {
            best_score = move_score;
            best_move = m;
        }

        if (move_score >= beta) {
            break;
        }

        alpha = std::max(alpha, move_score);
    }

    if (!legal) {
        const bool in_check = board.is_square_attacked(board.king_square(), static_cast<Color>(!board.color_to_move));
        return in_check ? CHECKMATE_SCORE - depth : 0;
    }

    tt_store(board.hash, depth, best_score, best_move, alpha_original, beta);

    return best_score;
}

Move search_root(const Board& board, const int depth) {

    MoveList moves;
    generate_moves(board, moves);
    Move best_move = NO_MOVE;
    int alpha = -INF_SCORE;

    //MVV LVA
    int scores[256];
    for (int i = 0; i < moves.count; i++)
        scores[i] = score_of(board, moves.moves[i]);

    Move tt_move = NO_MOVE; int ignored_score;
    tt_probe(board.hash, 1000, -INF_SCORE, INF_SCORE, ignored_score, tt_move);

    if (tt_move) {
        for (int i = 0; i<moves.count; i++) {
            if (moves.moves[i] == tt_move) {
                scores[i] = TT_BONUS_SCORE;
                break;
            }
        }
    }


    for (int i = 0; i < moves.count; i++) {

        //selection sort best move into i
        int best = i;
        for (int j = i+1; j < moves.count; j++) {
            if (scores[j] > scores[best]) {
                best = j;
            }
        }
        std::swap(moves.moves[i], moves.moves[best]);
        std::swap(scores[i], scores[best]);

        const Move m = moves.moves[i];
        Board board_copy = board;
        board_copy.make_move(m);

        if (!board_copy.last_move_was_legal()) {
            continue;
        }
        g_history[g_hist_count++] = board_copy.hash;
        const int score = -minimax(board_copy, depth-1, -INF_SCORE, -alpha, 1);
        g_hist_count--;
        if (score > alpha) {
            alpha = score;
            best_move = m;
        }
    }
    if (best_move) {
        tt_store(board.hash, depth, alpha, best_move, -INF_SCORE, INF_SCORE);
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


    const int max_depth = limits.depth > 0 ? limits.depth : 12;
    Move best_move = NO_MOVE;
    std::cout << "info string budget " << budget_ms << " elapsed " << elapsed_ms() << std::endl;
    for (int d = 1; d <= max_depth; d++) {
        best_move = search_root(board, d);
        if (budget_ms > 0 && elapsed_ms() > budget_ms / 2) break;
    }
    return best_move;
}

void history_reset(const uint64_t root_hash) {
    g_hist_count = 0;
    g_history[g_hist_count++] = root_hash;
}

void history_push(const uint64_t h) {
    g_history[g_hist_count++] = h;
}



