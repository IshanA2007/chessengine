//
// Created by Ishan Ajwani on 8/9/26.
//

#ifndef CHESSENGINE_MOVEGEN_H
#define CHESSENGINE_MOVEGEN_H
#include "board.h"
#include "moves.h"

void generate_moves(const Board& board, MoveList& moves);

void generate_knight_moves(const Board& board, MoveList& moves);

void generate_king_moves(const Board& board, MoveList& moves);

void generate_rook_moves(const Board& board, MoveList& moves);

void generate_bishop_moves(const Board& board, MoveList& moves);

void generate_queen_moves(const Board& board, MoveList& moves);

void generate_pawn_moves(const Board& board, MoveList& moves);

void generate_castling_moves(const Board& board, MoveList& moves);

uint64_t perft(const Board& board, int depth);

void handle_perft(const Board& board);

#endif //CHESSENGINE_MOVEGEN_H
