//
// Created by Ishan Ajwani on 8/9/26.
//

#include "uci.h"

#include <iostream>

#include "board.h"

#include <sstream>

#include "movegen.h"
#include "search.h"

Move parse_move(const Board& board, const std::string &token) {
    MoveList moves;
    generate_moves(board, moves);
    for (int i = 0; i < moves.count; i++) {
        const Move m = moves.moves[i];
        if (move_to_string(m) != token) {
            continue;
        }
        Board board_copy = board;
        board_copy.make_move(m);
        if (board_copy.last_move_was_legal()) {
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
        fen.pop_back();
        board.set_from_fen(fen);
    }
    while (ss >> token) {
        const Move m = parse_move(board, token);
        if (m == NO_MOVE) {
            std::cout << "bad move: " << token << "\n"; break;
        }
        board.make_move(m);
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
