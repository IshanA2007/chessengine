//
// Created by Ishan Ajwani on 8/9/26.
//

#include "movegen.h"

#include "attacks.h"

#include "bitboard.h"

#include <bit>


void generate_knight_moves(const Board& board, MoveList& moves) {
    const Color color_to_move = board.color_to_move;
    const Colored_Piece knight = make_piece(color_to_move, KNIGHT);
    Bitboard knight_bb = board.piece_bitboards[knight];
    const Bitboard nonfriendly = ~board.occupancy_bitboards[color_to_move];
    const Bitboard opponent = board.occupancy_bitboards[!color_to_move];
    //get all knight positions
    while (knight_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(knight_bb));
        Bitboard attacks = knight_attacks[from_square] & nonfriendly;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            const int flag = ((1ULL << to_square) & opponent) ? CAPTURE : QUIET;
            moves.moves[moves.count++] = make_move(from_square, to_square, flag);
            attacks &= attacks - 1;
        }
        knight_bb &= knight_bb - 1;
    }
}

void generate_king_moves(const Board& board, MoveList& moves) {
    const Color color_to_move = board.color_to_move;
    const Colored_Piece king = make_piece(color_to_move, KING);
    Bitboard king_bb = board.piece_bitboards[king];
    const Bitboard nonfriendly = ~board.occupancy_bitboards[color_to_move];
    const Bitboard opponent = board.occupancy_bitboards[!color_to_move];
    //get all king positions (should only be 1 but...)
    while (king_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(king_bb));
        Bitboard attacks = king_attacks[from_square] & nonfriendly;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            const int flag = ((1ULL << to_square) & opponent) ? CAPTURE : QUIET;
            moves.moves[moves.count++] = make_move(from_square, to_square, flag);
            attacks &= attacks - 1;
        }
        king_bb &= king_bb - 1;
    }
}

void generate_rook_moves(const Board& board, MoveList& moves) {
    const Color color_to_move = board.color_to_move;
    const Colored_Piece rook = make_piece(color_to_move, ROOK);
    Bitboard rook_bb = board.piece_bitboards[rook];
    const Bitboard occ = board.occupancy_bitboards[BOTH];
    const Bitboard nonfriendly = ~board.occupancy_bitboards[color_to_move];
    const Bitboard opponent = board.occupancy_bitboards[!color_to_move];
    //get all rook positions
    while (rook_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(rook_bb));
        Bitboard attacks = rook_attacks(from_square, occ) & nonfriendly;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            const int flag = ((1ULL << to_square) & opponent) ? CAPTURE : QUIET;
            moves.moves[moves.count++] = make_move(from_square, to_square, flag);
            attacks &= attacks - 1;
        }
        rook_bb &= rook_bb - 1;
    }
}

void generate_bishop_moves(const Board& board, MoveList& moves) {
    const Color color_to_move = board.color_to_move;
    const Colored_Piece bishop = make_piece(color_to_move, BISHOP);
    Bitboard bishop_bb = board.piece_bitboards[bishop];
    const Bitboard occ = board.occupancy_bitboards[BOTH];
    const Bitboard nonfriendly = ~board.occupancy_bitboards[color_to_move];
    const Bitboard opponent = board.occupancy_bitboards[!color_to_move];
    //get all bishop positions
    while (bishop_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(bishop_bb));
        Bitboard attacks = bishop_attacks(from_square, occ) & nonfriendly;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            const int flag = ((1ULL << to_square) & opponent) ? CAPTURE : QUIET;
            moves.moves[moves.count++] = make_move(from_square, to_square, flag);
            attacks &= attacks - 1;
        }
        bishop_bb &= bishop_bb - 1;
    }
}

void generate_queen_moves(const Board& board, MoveList& moves) {
    const Color color_to_move = board.color_to_move;
    const Colored_Piece queen = make_piece(color_to_move, QUEEN);
    Bitboard queen_bb = board.piece_bitboards[queen];
    const Bitboard occ = board.occupancy_bitboards[BOTH];
    const Bitboard nonfriendly = ~board.occupancy_bitboards[color_to_move];
    const Bitboard opponent = board.occupancy_bitboards[!color_to_move];
    //get all queen positions
    while (queen_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(queen_bb));
        const Bitboard battacks = bishop_attacks(from_square, occ);
        const Bitboard rattacks = rook_attacks(from_square, occ);
        Bitboard attacks = (battacks | rattacks) & nonfriendly;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            const int flag = ((1ULL << to_square) & opponent) ? CAPTURE : QUIET;
            moves.moves[moves.count++] = make_move(from_square, to_square, flag);
            attacks &= attacks - 1;
        }
        queen_bb &= queen_bb - 1;
    }
}

void generate_pawn_moves(const Board& board, MoveList& moves) {
    const Color color_to_move = board.color_to_move;
    const Colored_Piece pawn = make_piece(color_to_move, PAWN);

    const Bitboard empty  = ~board.occupancy_bitboards[BOTH];
    const Bitboard opponent = board.occupancy_bitboards[!color_to_move];
    const int fwd = (color_to_move == WHITE) ? 8 : -8;

    Bitboard pawn_bb = board.piece_bitboards[pawn];
    //gen 1-sq pushes
    Bitboard pawn_bb_1push = shift_signed(pawn_bb, fwd) & empty;

    //gen promo-specific pushes
    const Bitboard promo_rank = (color_to_move == WHITE) ? RANK_8 : RANK_1;
    Bitboard pawn_promos_bb = pawn_bb_1push & promo_rank;
    pawn_bb_1push &= ~promo_rank;

    //gen 2-sq pushes
    Bitboard pawn_bb_2push = shift_signed(pawn_bb_1push, fwd) & empty;
    pawn_bb_2push &= color_to_move == WHITE ? RANK_4 : RANK_5;

    while (pawn_bb_1push) {
        const auto to_square = static_cast<Square>(std::countr_zero(pawn_bb_1push));
        const auto from_square = static_cast<Square>(to_square-fwd);
        moves.moves[moves.count++] = make_move(from_square, to_square, QUIET);
        pawn_bb_1push &= pawn_bb_1push - 1;
    }

    while (pawn_promos_bb) {
        const auto to_square = static_cast<Square>(std::countr_zero(pawn_promos_bb));
        const auto from_square = static_cast<Square>(to_square-fwd);
        moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_B);
        moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_N);
        moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_Q);
        moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_R);
        pawn_promos_bb &= pawn_promos_bb - 1;
    }

    while (pawn_bb_2push) {
        const auto to_square = static_cast<Square>(std::countr_zero(pawn_bb_2push));
        const auto from_square = static_cast<Square>(to_square-fwd-fwd);
        moves.moves[moves.count++] = make_move(from_square, to_square, DOUBLE_PUSH);
        pawn_bb_2push &= pawn_bb_2push - 1;
    }

    //gen attacks
    const Bitboard capture_promo_rank = (color_to_move == WHITE) ? RANK_7 : RANK_2;
    Bitboard pawn_capture_promos_bb = pawn_bb & capture_promo_rank;
    pawn_bb &= ~capture_promo_rank;

    //check non-prmoting caps first
    while (pawn_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(pawn_bb));
        Bitboard attacks = pawn_attacks[color_to_move][from_square] & opponent;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            moves.moves[moves.count++] = make_move(from_square, to_square, CAPTURE);
            attacks &= attacks - 1;
        }
        pawn_bb &= pawn_bb - 1;
    }

    //now check promoting caps
    while (pawn_capture_promos_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(pawn_capture_promos_bb));
        Bitboard attacks = pawn_attacks[color_to_move][from_square] & opponent;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_B_CAPTURE);
            moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_N_CAPTURE);
            moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_Q_CAPTURE);
            moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_R_CAPTURE);

            attacks &= attacks - 1;
        }
        pawn_capture_promos_bb &= pawn_capture_promos_bb - 1;
    }

    //gen en-passant captures
    if (Square ep = board.en_passant_square; ep != NO_SQUARE) {
        Bitboard ep_attacks_bb = pawn_attacks[!color_to_move][ep] & board.piece_bitboards[pawn];
        while (ep_attacks_bb) {
            const auto from_square = static_cast<Square>(std::countr_zero(ep_attacks_bb));
            moves.moves[moves.count++] = make_move(from_square, ep, EN_PASSANT);
            ep_attacks_bb &= ep_attacks_bb - 1;
        }
    }
}

void generate_castling_moves(const Board& board, MoveList& moves) {
    const uint8_t castling_rights = board.castling_rights;
    const Bitboard occ = board.occupancy_bitboards[BOTH];
    if (board.color_to_move == WHITE) {
        const bool in_check = board.is_square_attacked(E1, BLACK);
        if ((WHITE_OO & castling_rights) && !(WHITE_OO_EMPTY & occ) && !in_check && !board.is_square_attacked(F1, BLACK) && !board.is_square_attacked(G1, BLACK)) {
            moves.moves[moves.count++] = make_move(E1, G1, CASTLE_KINGSIDE);
        }
        if ((WHITE_OOO & castling_rights) && !(WHITE_OOO_EMPTY & occ) && !in_check && !board.is_square_attacked(D1, BLACK) && !board.is_square_attacked(C1, BLACK)) {
            moves.moves[moves.count++] = make_move(E1, C1, CASTLE_QUEENSIDE);
        }
    }
    else {
        const bool in_check = board.is_square_attacked(E8, WHITE);
        if ((BLACK_OO & castling_rights) && !(BLACK_OO_EMPTY & occ) && !in_check && !board.is_square_attacked(F8, WHITE) && !board.is_square_attacked(G8, WHITE)) {
            moves.moves[moves.count++] = make_move(E8, G8, CASTLE_KINGSIDE);
        }
        if ((BLACK_OOO & castling_rights) && !(BLACK_OOO_EMPTY & occ) && !in_check && !board.is_square_attacked(D8, WHITE) && !board.is_square_attacked(C8, WHITE)) {
            moves.moves[moves.count++] = make_move(E8, C8, CASTLE_QUEENSIDE);
        }
    }
}

void generate_moves(const Board& board, MoveList& moves) {
    //gen psuedo-legal moves
    generate_knight_moves(board, moves);

    generate_king_moves(board, moves);

    generate_rook_moves(board, moves);

    generate_bishop_moves(board, moves);

    generate_queen_moves(board, moves);

    generate_pawn_moves(board, moves);

    generate_castling_moves(board, moves);
}

uint64_t perft(Board& board, int depth) {
    if (!depth) {
        return 1;
    }
    uint64_t nodes = 0;
    MoveList moves;
    generate_moves(board, moves);

    for (int i = 0; i < moves.count; i++) {
        const Move move = moves.moves[i];
        Undo u;
        board.make_move(move, u);
        if (!board.last_move_was_legal()) {
            board.unmake_move(move, u);
            continue;
        }
        nodes += perft(board, depth-1);
        board.unmake_move(move, u);
    }

    return nodes;
}

void handle_perft(const Board& board) {
    for (int i = 0; i <= 5; i++) {
        Board perft_board = board;
        const auto start = std::chrono::steady_clock::now();

        const uint64_t nodes = perft(perft_board, i);

        const auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                            std::chrono::steady_clock::now() - start).count();
        const uint64_t nps = us > 0 ? nodes * 1'000'000 / us : 0;

        std::cout << "depth " << i
                  << " nodes " << nodes
                  << " time " << (us / 1000) << "ms"
                  << " nps " << nps << std::endl;
    }
}

void generate_captures(const Board &board, MoveList &moves) {
    //gen knight captures
    const Color color_to_move = board.color_to_move;
    const Colored_Piece knight = make_piece(color_to_move, KNIGHT);
    Bitboard knight_bb = board.piece_bitboards[knight];
    const Bitboard opponent = board.occupancy_bitboards[!color_to_move];
    //get all knight positions
    while (knight_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(knight_bb));
        Bitboard attacks = knight_attacks[from_square] & opponent;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            moves.moves[moves.count++] = make_move(from_square, to_square, CAPTURE);
            attacks &= attacks - 1;
        }
        knight_bb &= knight_bb - 1;
    }

    //gen king captures
    const Colored_Piece king = make_piece(color_to_move, KING);
    Bitboard king_bb = board.piece_bitboards[king];
    //get all king positions (should only be 1 but...)
    while (king_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(king_bb));
        Bitboard attacks = king_attacks[from_square] & opponent;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            moves.moves[moves.count++] = make_move(from_square, to_square, CAPTURE);
            attacks &= attacks - 1;
        }
        king_bb &= king_bb - 1;
    }

    //gen rook moves
    const Colored_Piece rook = make_piece(color_to_move, ROOK);
    Bitboard rook_bb = board.piece_bitboards[rook];
    const Bitboard occ = board.occupancy_bitboards[BOTH];
    //get all rook positions
    while (rook_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(rook_bb));
        Bitboard attacks = rook_attacks(from_square, occ) & opponent;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            moves.moves[moves.count++] = make_move(from_square, to_square, CAPTURE);
            attacks &= attacks - 1;
        }
        rook_bb &= rook_bb - 1;
    }

    //gen bishop moves
    const Colored_Piece bishop = make_piece(color_to_move, BISHOP);
    Bitboard bishop_bb = board.piece_bitboards[bishop];
    //get all bishop positions
    while (bishop_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(bishop_bb));
        Bitboard attacks = bishop_attacks(from_square, occ) & opponent;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            moves.moves[moves.count++] = make_move(from_square, to_square, CAPTURE);
            attacks &= attacks - 1;
        }
        bishop_bb &= bishop_bb - 1;
    }

    //gen queen moves
    const Colored_Piece queen = make_piece(color_to_move, QUEEN);
    Bitboard queen_bb = board.piece_bitboards[queen];
    //get all queen positions
    while (queen_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(queen_bb));
        const Bitboard battacks = bishop_attacks(from_square, occ);
        const Bitboard rattacks = rook_attacks(from_square, occ);
        Bitboard attacks = (battacks | rattacks) & opponent;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            moves.moves[moves.count++] = make_move(from_square, to_square, CAPTURE);
            attacks &= attacks - 1;
        }
        queen_bb &= queen_bb - 1;
    }

    //gen pawn moves
    const Colored_Piece pawn = make_piece(color_to_move, PAWN);

    const Bitboard empty  = ~board.occupancy_bitboards[BOTH];
    const int fwd = (color_to_move == WHITE) ? 8 : -8;

    Bitboard pawn_bb = board.piece_bitboards[pawn];
    //gen 1-sq pushes
    Bitboard pawn_bb_1push = shift_signed(pawn_bb, fwd) & empty;

    //gen promo-specific pushes
    const Bitboard promo_rank = (color_to_move == WHITE) ? RANK_8 : RANK_1;
    Bitboard pawn_promos_bb = pawn_bb_1push & promo_rank;
    pawn_bb_1push &= ~promo_rank;

    while (pawn_promos_bb) {
        const auto to_square = static_cast<Square>(std::countr_zero(pawn_promos_bb));
        const auto from_square = static_cast<Square>(to_square-fwd);
        moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_B);
        moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_N);
        moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_Q);
        moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_R);
        pawn_promos_bb &= pawn_promos_bb - 1;
    }

    //gen attacks
    const Bitboard capture_promo_rank = (color_to_move == WHITE) ? RANK_7 : RANK_2;
    Bitboard pawn_capture_promos_bb = pawn_bb & capture_promo_rank;
    pawn_bb &= ~capture_promo_rank;

    //check non-prmoting caps first
    while (pawn_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(pawn_bb));
        Bitboard attacks = pawn_attacks[color_to_move][from_square] & opponent;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            moves.moves[moves.count++] = make_move(from_square, to_square, CAPTURE);
            attacks &= attacks - 1;
        }
        pawn_bb &= pawn_bb - 1;
    }

    //now check promoting caps
    while (pawn_capture_promos_bb) {
        const auto from_square = static_cast<Square>(std::countr_zero(pawn_capture_promos_bb));
        Bitboard attacks = pawn_attacks[color_to_move][from_square] & opponent;
        while (attacks) {
            const auto to_square = static_cast<Square>(std::countr_zero(attacks));
            moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_B_CAPTURE);
            moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_N_CAPTURE);
            moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_Q_CAPTURE);
            moves.moves[moves.count++] = make_move(from_square, to_square, PROMO_R_CAPTURE);

            attacks &= attacks - 1;
        }
        pawn_capture_promos_bb &= pawn_capture_promos_bb - 1;
    }

    //gen en-passant captures
    if (const Square ep = board.en_passant_square; ep != NO_SQUARE) {
        Bitboard ep_attacks_bb = pawn_attacks[!color_to_move][ep] & board.piece_bitboards[pawn];
        while (ep_attacks_bb) {
            const auto from_square = static_cast<Square>(std::countr_zero(ep_attacks_bb));
            moves.moves[moves.count++] = make_move(from_square, ep, EN_PASSANT);
            ep_attacks_bb &= ep_attacks_bb - 1;
        }
    }
}
