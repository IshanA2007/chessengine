//
// Created by Ishan Ajwani on 8/9/26.
//

#include "uci.h"

#include <iostream>

#include "board.h"

#include <sstream>
#include <chrono>

#include "movegen.h"
#include "search.h"
#include "transposition.h"

Move parse_move(const Board& board, const std::string &token) {
    MoveList moves;
    generate_moves(board, moves);
    for (int i = 0; i < moves.count; i++) {
        const Move m = moves.moves[i];
        if (move_to_string(m) != token) {
            continue;
        }
        Board temp = board;
        Undo u;
        temp.make_move(m, u);
        if (temp.last_move_was_legal()) {
            return m;
        }
    }
    return NO_MOVE;
}

void handle_position(Board& board, std::istringstream& ss) {
    std::string token;
    ss >> token;

    if (token == "startpos") {
        board.set_from_fen(START_FEN);
        ss >> token;
    }
    else if (token == "fen") {
        std::string fen;
        while (ss >> token && token != "moves") {
            fen += token + " ";
        }
        if (!fen.empty()) fen.pop_back();
        board.set_from_fen(fen);
    }
    history_reset(board.hash);
    while (ss >> token) {
        const Move m = parse_move(board, token);
        if (m == NO_MOVE) {
            std::cout << "bad move: " << token << "\n"; break;
        }
        Undo u;
        board.make_move(m, u);
        history_push(board.hash);
    }
}

void handle_go(const Board& board, std::istringstream& ss) {
    GoLimits limits;
    std::string token;
    while (ss >> token) {
        if (token == "depth") {
            ss >> limits.depth;
        }
        else if (token == "movetime") {
            ss >> limits.movetime;
        }
        else if (token == "wtime") {
            ss >> limits.wtime;
        }
        else if (token == "btime") {
            ss >> limits.btime;
        }
        else if (token == "winc") {
            ss >> limits.winc;
        }
        else if (token == "binc") {
            ss >> limits.binc;
        }
        else if (token == "infinite") {
            limits.infinite = true;
        }
    }
    const Move m = search(board, limits);
    // UCI null move: no legal reply (mate or stalemate)
    std::cout << "bestmove " << (m == NO_MOVE ? "0000" : move_to_string(m)) << std::endl;
}

// Fixed suite + fixed depth = a reproducible node count. The node total must be
// identical across runs; anything else means the search depends on wall-clock or
// on state that survives between positions.
static constexpr const char* BENCH_FENS[] = {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
    "4k3/8/8/8/8/8/8/4K2R w K - 0 1",
    "8/8/8/4k3/8/8/4P3/4K3 w - - 0 1",
};

void handle_bench(const int depth) {
    uint64_t total_nodes = 0;
    const auto start = std::chrono::steady_clock::now();

    for (const char* fen : BENCH_FENS) {
        Board board{};
        board.set_from_fen(fen);
        // Clear the table between positions so each one is independent.
        tt_clear();
        history_reset(board.hash);

        GoLimits limits;
        limits.depth = depth;
        search(board, limits);
        total_nodes += nodes_searched();
    }

    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();

    std::cout << "\n==========================="
              << "\nBench depth  : " << depth
              << "\nTotal nodes  : " << total_nodes
              << "\nTime         : " << ms << " ms"
              << "\nNPS          : " << (ms > 0 ? total_nodes * 1000 / ms : 0)
              << "\n===========================" << std::endl;
}
