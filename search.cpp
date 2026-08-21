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

static constexpr int HISTORY_MAX = 16'000;
static constexpr int LOSING_CAPTURE_BONUS = 80'000;
static Move killers[MAX_PLY][2];
static int history[2][64][64];

//inloop abort
static bool g_stop = false;
static int64_t g_hard_ms = -1;

static int64_t elapsed_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - g_search_start).count();
}

static inline bool check_time() {
    if (g_stop) return true;
    if (g_hard_ms > 0 && (g_nodes & 2047) == 0 && elapsed_ms() >= g_hard_ms)
        g_stop = true;
    return g_stop;
}

static void age_history() {
    for (auto& color_plane : history)
        for (auto& from_row : color_plane)
            for (int& h : from_row)
                h /= 2;
}

static void clear_killers() {
    for (auto& ply_killers : killers) {
        ply_killers[0] = NO_MOVE;
        ply_killers[1] = NO_MOVE;
    }
}

static void clear_history() {
    std::memset(history, 0, sizeof(history));
}



static void score_moves(const Board& board, const MoveList& moves, int* scores, const Move tt_move, const int ply) {
    for (int i = 0; i < moves.count; i++) {
        Move m = moves.moves[i];
        if (m == tt_move) {
            scores[i] = TT_BONUS_SCORE;
        }
        else if (is_capture(m) || is_promotion(m)) {
            if (is_promotion(m) || see(board, m) >= 0) {
                scores[i] = mvv_score_of(board, m);
            }
            else {
                scores[i] = LOSING_CAPTURE_BONUS + mvv_score_of(board, m) - CAPTURE_BONUS;
            }

        }
        else if (m == killers[ply][0]) {
            scores[i] = 90'000;
        }
        else if (m == killers[ply][1]) {
            scores[i] = 89'999;
        }
        else {
            scores[i] = history[board.color_to_move][from_square(m)][to_square(m)];
        }
    }
}

int quiescence(Board& board, int alpha, const int beta, const int ply) {
    g_nodes++;
    if (check_time()) return 0;
    if (ply >= MAX_PLY) {
        return eval(board);
    }

    const bool in_check = board.is_square_attacked(
        board.king_square(), static_cast<Color>(!board.color_to_move));

    MoveList moves;
    if (in_check) {
        generate_moves(board, moves);
    }
    else {
        const int stand_pat = eval(board);
        if (stand_pat >= beta) return stand_pat;
        if (stand_pat > alpha) alpha = stand_pat;
        generate_captures(board, moves);
    }

    int legal = 0;

    //MVV-LVA
    int scores[256];
    for (int i = 0; i < moves.count; i++)
        scores[i] = mvv_score_of(board, moves.moves[i]);

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

        if (!in_check && see(board, m) < 0) {
            continue;
        }

        Undo u;
        board.make_move(m, u);
        if (!board.last_move_was_legal()) {
            board.unmake_move(m, u);
            continue;
        }
        legal++;

        const int score = -quiescence(board, -beta, -alpha, ply+1);
        board.unmake_move(m, u);
        if (score >= beta) {
            return score;
        }
        if (score > alpha) {
            alpha = score;
        }
    }
    if (in_check && !legal) {
        return -CHECKMATE_SCORE + ply;
    }
    return alpha;


}


static int minimax(Board& board, const int depth, int alpha, const int beta, const int ply, const bool can_null) {
    g_nodes++;
    if (check_time()) return 0;

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

    if (tt_probe(board.hash, depth, ply, alpha_original, beta, tt_score, tt_move)) {
        return tt_score;
    }
    if (depth <= 0) {
        return quiescence(board, alpha, beta, ply);
    }

    Color us = board.color_to_move;
    const bool has_pieces =
    (board.piece_bitboards[make_piece(us, KNIGHT)] |
     board.piece_bitboards[make_piece(us, BISHOP)] |
     board.piece_bitboards[make_piece(us, ROOK)]   |
     board.piece_bitboards[make_piece(us, QUEEN)]) != 0;

    bool in_check = board.is_square_attacked(board.king_square(), static_cast<Color>(!us));


    //null-move
    if (can_null && depth >= 3 && !in_check && has_pieces){
        NullUndo nu;
        board.make_null_move(nu);
        const int score = -minimax(board, depth - 3, -beta, -beta + 1, ply + 1, false);
        board.unmake_null_move(nu);
        if (score >= beta) {
            return score;
        }
    }

    MoveList moves;
    generate_moves(board, moves);
    int legal = 0;
    int best_score = -INF_SCORE;
    Move best_move = NO_MOVE;

    //move ordering (mvv lva, transposition, killer moves, history)
    int scores[256];
    score_moves(board, moves, scores, tt_move, ply);

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
        Undo u;
        board.make_move(m, u);
        if (!board.last_move_was_legal()) {
            board.unmake_move(m, u);
            continue;
        }
        legal++;
        g_history[g_hist_count++] = board.hash;

        int move_score = 0;

        //late-move reduction
        bool needs_full_search = true;
        if (i >= 4 && depth >= 3 && !is_capture(m) && !is_promotion(m) && !in_check && !board.is_square_attacked(board.king_square(), us)) { //late and not too shallow
            constexpr int red = 1; //tune later this is naive
            move_score = -minimax(board, depth-1-red, -beta, -alpha, ply+1, true);
            if (move_score <= alpha) {
                needs_full_search = false;
            }
        }

        if (needs_full_search) {
            move_score = -minimax(board, depth - 1, -beta, -alpha, ply+1, true);
        }

        g_hist_count--;
        board.unmake_move(m, u);

        if (move_score > best_score) {
            best_score = move_score;
            best_move = m;
        }

        if (move_score >= beta) {
            if (!is_capture(m) && !is_promotion(m)) {
                if (killers[ply][0] != m) {
                    killers[ply][1] = killers[ply][0];
                    killers[ply][0] = m;
                }

                history[us][from_square(m)][to_square(m)] += depth * depth;
                if (history[us][from_square(m)][to_square(m)] > HISTORY_MAX) {
                    age_history();
                }
            }
            break;
        }

        alpha = std::max(alpha, move_score);
    }

    if (!legal) {
        return in_check ? -CHECKMATE_SCORE + ply : 0;
    }
    if (!g_stop) {
        tt_store(board.hash, depth, ply, best_score, best_move, alpha_original, beta);
    }


    return best_score;
}

bool search_root(Board& board, const int depth, Move& out_best) {

    MoveList moves;
    generate_moves(board, moves);
    Move best_move = NO_MOVE;
    int alpha = -INF_SCORE;

    Move tt_move = NO_MOVE; int ignored_score;
    tt_probe(board.hash, 1000, 0, -INF_SCORE, INF_SCORE, ignored_score, tt_move);



    //MVV LVA
    int scores[256];
    score_moves(board, moves, scores, tt_move, 0);




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
        Undo u;
        board.make_move(m, u);
        if (!board.last_move_was_legal()) {
            board.unmake_move(m, u);
            continue;
        }
        g_history[g_hist_count++] = board.hash;
        const int score = -minimax(board, depth-1, -INF_SCORE, -alpha, 1, true);
        g_hist_count--;
        board.unmake_move(m, u);
        if (g_stop) {
            out_best = best_move;
            return false;
        }
        if (score > alpha) {
            alpha = score;
            best_move = m;
        }
    }
    if (best_move) {
        tt_store(board.hash, depth, 0 , alpha, best_move, -INF_SCORE, INF_SCORE);
    }

    std::cout << "info depth " << depth << " nodes " << g_nodes << " score ";
    if (alpha >= MATE_BOUND)
        std::cout << "mate " << (CHECKMATE_SCORE - alpha + 1) / 2;
    else if (alpha <= -MATE_BOUND)
        std::cout << "mate " << -(CHECKMATE_SCORE + alpha) / 2;
    else
        std::cout << "cp " << alpha;
    std::cout << " pv " << move_to_string(best_move) << std::endl;
    out_best = best_move;
    return true;
}

Move search(const Board& board, const GoLimits& limits) {
    g_nodes = 0;
    g_search_start = std::chrono::steady_clock::now();
    g_stop  = false;

    clear_history();
    clear_killers();

    int64_t soft_ms = -1;
    g_hard_ms = -1;

    if (limits.movetime > 0) {
        soft_ms = g_hard_ms = limits.movetime - 30;
    } else {
        const int mytime = board.color_to_move == WHITE ? limits.wtime : limits.btime;
        const int myinc  = board.color_to_move == WHITE ? limits.winc  : limits.binc;
        if (mytime > 0) {
            soft_ms   = mytime / 25 + myinc / 2 - 30;
            g_hard_ms = std::min<int64_t>(mytime / 4, mytime - 50);
        }
    }

    const int max_depth = limits.depth > 0 ? limits.depth : MAX_PLY - 1;
    Move best_move = NO_MOVE;
    Board search_board = board;

    for (int d = 1; d <= max_depth; d++) {
        Move iter_move = NO_MOVE;
        if (search_root(search_board, d, iter_move)) {
            best_move = iter_move;
        } else {
            if (best_move == NO_MOVE) best_move = iter_move;
            break;
        }
        if (soft_ms > 0 && elapsed_ms() > soft_ms / 2) break;
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

uint64_t nodes_searched() {
    return g_nodes;
}



