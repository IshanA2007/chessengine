//
// Created by Ishan Ajwani on 8/8/26.
//

#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H
#include <string>

#include "attacks.h"
#include "moves.h"
#include "types.h"
#include "zobrist.h"
struct Undo {
    Colored_Piece captured;
    uint8_t castling_rights;
    Square ep_square;
    uint8_t fifty_clock;
    uint64_t hash;
    uint16_t fullmove_number;
};

struct NullUndo {
    Square ep_square;
    uint64_t hash;
};

struct Board {
    uint64_t hash;
    Bitboard piece_bitboards[12];
    Bitboard occupancy_bitboards[3];

    Colored_Piece mailbox[64];

    Color color_to_move;

    // smallest 4 bits matter: abcd -> cd rep white castling, ab rep black castling
    uint8_t castling_rights;

    Square en_passant_square;

    uint8_t fifty_clock;

    // necessary for FEN extraction
    uint16_t fullmove_number;

    void set_from_fen(const std::string& fen);

    void print() const;

    [[nodiscard]] inline bool is_square_attacked(const Square square, const Color by) const {
        const Bitboard occ = occupancy_bitboards[BOTH];
        const Bitboard queen_bb = piece_bitboards[make_piece(by, QUEEN)];
        if (pawn_attacks[!by][square] & piece_bitboards[make_piece(by, PAWN)]) { return true;}
        if (knight_attacks[square] & piece_bitboards[make_piece(by, KNIGHT)]) { return true;}
        if (king_attacks[square] & piece_bitboards[make_piece(by, KING)]) { return true;}
        if (rook_attacks(square, occ) & (piece_bitboards[make_piece(by, ROOK)] | queen_bb)) { return true;}
        if (bishop_attacks(square, occ) & (piece_bitboards[make_piece(by, BISHOP)] | queen_bb)) { return true;}
        return false;
    }

    Bitboard attackers_to(Square square, Bitboard occupancy) const;

    inline void put_piece(const Colored_Piece piece, const Square square) {
        mailbox[square] = piece;
        piece_bitboards[piece] |= (1ULL << square);
        occupancy_bitboards[color_of(piece)] |= (1ULL << square);
        occupancy_bitboards[BOTH] |= (1ULL << square);
        hash ^= zobrist_pieces[piece][square];
    }

    inline void remove_piece(const Square square) {
        const Colored_Piece piece = mailbox[square];
        mailbox[square] = NO_PIECE;
        piece_bitboards[piece] ^= (1ULL << square);
        occupancy_bitboards[color_of(piece)] ^= (1ULL << square);
        occupancy_bitboards[BOTH] ^= (1ULL << square);
        hash ^= zobrist_pieces[piece][square];
    }

    void make_move(Move m, Undo& u);

    void unmake_move(Move m, const Undo &u);

    void make_null_move(NullUndo& u);

    void unmake_null_move(const NullUndo& u);

    [[nodiscard]] inline Square enemy_king_square() const {
        return static_cast<Square>(std::countr_zero(piece_bitboards[make_piece(static_cast<Color>(!color_to_move), KING)]));
    }

    [[nodiscard]] inline Square king_square() const {
        return static_cast<Square>(std::countr_zero(piece_bitboards[make_piece(color_to_move, KING)]));
    }

    [[nodiscard]] inline bool last_move_was_legal() const {
        return !is_square_attacked(enemy_king_square(), color_to_move);
    }

    [[nodiscard]] uint64_t compute_hash_from_scratch() const;
};


#endif //CHESSENGINE_BOARD_H
