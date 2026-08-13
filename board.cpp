//
// Created by Ishan Ajwani on 8/8/26.
//

#include "board.h"

#include <iostream>
#include <sstream>

#include "moves.h"

using namespace std;

constexpr std::array<uint64_t, 64> rights_mask = []() {
    std::array<uint64_t, 64> table{};
    for (auto& mask : table) {
        mask = 0b1111;
    }

    table[E1] = 0b1100; // ~(WHITE_OO | WHITE_OOO)
    table[H1] = 0b1110; // ~WHITE_OO
    table[A1] = 0b1101; // ~WHITE_OOO
    table[E8] = 0b0011; // ~(BLACK_OO | BLACK_OOO)
    table[H8] = 0b1011; // ~BLACK_OO
    table[A8] = 0b0111; // ~BLACK_OOO

    return table;
}();

//ex fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
void Board::set_from_fen(const string& fen) {
    //get parts
    vector<string> parts(6);
    stringstream sstream(fen);
    string token;

    for (int i = 0; i < 6; i++) {
        getline(sstream, token, ' ');
        parts[i] = token;
    }

    const unordered_map<string, Color> color_map = {
        {"w", WHITE},
        {"b", BLACK}
    };

    const unordered_map<char, CastlingRights> castling_map = {
        {'k', BLACK_OO},
        {'q', BLACK_OOO},
        {'K', WHITE_OO},
        {'Q', WHITE_OOO}
    };

    //set color to move
    color_to_move = color_map.at(parts[1]);

    //set castling rights
    castling_rights = 0;
    if (parts[2] != "-") {
        for (char c : parts[2]) {
            castling_rights |= castling_map.at(c);
        }
    }

    //set en passant
    en_passant_square = NO_SQUARE;
    if (parts[3] != "-") {
        int file = parts[3][0] - 'a';
        int rank = parts[3][1] - '1';
        en_passant_square = static_cast<Square>(file+8*rank);
    }

    //set clocks
    fifty_clock = static_cast<uint8_t>(stoi(parts[4]));
    fullmove_number = static_cast<uint16_t>(stoi(parts[5]));

    //set bitboards up
    for (int i = 0; i < 12; i++) {
        piece_bitboards[i] = 0;
    }
    for (int i = 0; i < 3; i++) {
        occupancy_bitboards[i] = 0;
    }
    const unordered_map<char, Colored_Piece> piece_map = {
        {'P', WP}, {'N', WN}, {'B', WB}, {'R', WR}, {'Q', WQ}, {'K', WK},
        {'p', BP}, {'n', BN}, {'b', BB}, {'r', BR}, {'q', BQ}, {'k', BK}
    };

    for (int i = 0; i < 64; i++) {
        mailbox[i] = NO_PIECE;
    }
    int file = 0;
    int rank = 7;
    for (char c : parts[0]) {
        if (c == '/') {
            rank--;
            file = 0;
        }
        else if (isdigit(c)) {
            file += c - '0';
        }
        else {
            int square = file + 8 * rank;
            piece_bitboards[piece_map.at(c)] |= 1ULL << square;
            mailbox[square] = piece_map.at(c);
            file++;
        }
    }

    for (int i = 0; i < 12; i++) {
        Color color = BLACK;
        if (i < 6) {
            color = WHITE;
        }
        occupancy_bitboards[color] |= piece_bitboards[i];
        occupancy_bitboards[BOTH] |= piece_bitboards[i];
    }
    hash = compute_hash_from_scratch();
}

void Board::print() const {
    // piece index -> display char
    static constexpr char piece_chars[12] = {
        'P', 'N', 'B', 'R', 'Q', 'K',
        'p', 'n', 'b', 'r', 'q', 'k'
    };

    auto square_to_string = [](const Square sq) -> string {
        if (sq == NO_SQUARE) return "-";
        const int file = static_cast<int>(sq) % 8;
        const int rank = static_cast<int>(sq) / 8;
        return string(1, static_cast<char>('a' + file)) +
               string(1, static_cast<char>('1' + rank));
    };

    auto castling_to_string = [](uint8_t rights) -> string {
        if (rights == 0) return "-";
        string s;
        if (rights & WHITE_OO)  s += 'K';
        if (rights & WHITE_OOO) s += 'Q';
        if (rights & BLACK_OO)  s += 'k';
        if (rights & BLACK_OOO) s += 'q';
        return s;
    };

    // --- board ---
    cout << "  a b c d e f g h\n";

    for (int rank = 7; rank >= 0; --rank) {
        cout << rank + 1 << ' ';

        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            char ch = '.';

            for (int piece = WP; piece <= BK; ++piece) {
                if (piece_bitboards[piece] & (1ULL << sq)) {
                    ch = piece_chars[piece];
                    break;
                }
            }

            cout << ch << ' ';
        }

        cout << rank + 1 << '\n';
    }

    cout << "  a b c d e f g h\n\n";

    // --- metadata ---
    cout << "Color to move: "
         << (color_to_move == WHITE ? "White" : "Black") << '\n';

    cout << "Castling: " << castling_to_string(castling_rights) << '\n';
    cout << "En passant: " << square_to_string(en_passant_square) << '\n';
    cout << "Halfmove clock: " << static_cast<int>(fifty_clock) << '\n';
    cout << "Fullmove number: " << fullmove_number << '\n';
}

void Board::make_move(const Move m, Undo& u) {
    const Square from = from_square(m);
    const Square to = to_square(m);
    const int flag = flags(m);
    const Colored_Piece mover = mailbox[from];
    const Color us = color_to_move;
    const int offset = us == WHITE ? 8 : -8;

    u.castling_rights = castling_rights;
    u.ep_square = en_passant_square;
    u.fifty_clock = fifty_clock;
    u.fullmove_number = fullmove_number;
    u.hash = hash;
    u.captured = NO_PIECE;
    //move pieces bit
    remove_piece(from);
    if (is_capture(m)) {
        Square capture_square;
        if (flag == EN_PASSANT) {
            capture_square = static_cast<Square>(to - offset);
        }
        else {
            capture_square = to;

        }
        u.captured = mailbox[capture_square];
        remove_piece(capture_square);
    }

    const Colored_Piece placed = is_promotion(m) ? make_piece(us, promotion_of(m)) : mover;
    put_piece(placed, to);

    //handle en passant sq
    if (en_passant_square != NO_SQUARE) {
        hash ^= zobrist_ep_file[file_of(en_passant_square)];
    }
    en_passant_square = flag == DOUBLE_PUSH ? static_cast<Square>(to - offset) : NO_SQUARE;
    if (en_passant_square != NO_SQUARE) {
        hash ^= zobrist_ep_file[file_of(en_passant_square)];
    }
    //castling rights
    if (flag == CASTLE_KINGSIDE) {
        if (us == WHITE) {
            remove_piece(H1);
            put_piece(WR, F1);
        }
        else {
            remove_piece(H8);
            put_piece(BR, F8);
        }
    }
    else if (flag == CASTLE_QUEENSIDE) {
        if (us == WHITE) {
            remove_piece(A1);
            put_piece(WR, D1);
        }
        else {
            remove_piece(A8);
            put_piece(BR, D8);
        }
    }
    uint8_t old_rights = castling_rights;
    castling_rights &= (rights_mask[from] & rights_mask[to]);
    hash ^= zobrist_castling[old_rights] ^ zobrist_castling[castling_rights];

    //tick clocks
    if (us == BLACK) {
        fullmove_number++;
    }
    if (type_of(mover) == PAWN || is_capture(m)) {
        fifty_clock = 0;
    }
    else {
        fifty_clock++;
    }

    //flip side to move
    color_to_move = us == WHITE ? BLACK : WHITE;
    hash ^= zobrist_side;

    //assert(hash == compute_hash_from_scratch());
}

void Board::unmake_move(Move m, const Undo& u) {
    color_to_move = static_cast<Color>(!color_to_move);
    const Color us = color_to_move;
    const Square from = from_square(m);
    const Square to = to_square(m);
    const int offset = (us == WHITE) ? 8 : -8;

    if (is_promotion(m)) {
        remove_piece(to);
        put_piece(make_piece(us, PAWN), from);
    }
    else {
        const Colored_Piece moved = mailbox[to];
        remove_piece(to);
        put_piece(moved, from);
    }

    if (flags(m) == EN_PASSANT) {
        put_piece(u.captured, static_cast<Square>(to-offset));
    }
    else if (is_capture(m)) {
        put_piece(u.captured, to);
    }

    if (flags(m) == CASTLE_KINGSIDE) {
        if (us == WHITE) {
            remove_piece(F1); put_piece(WR, H1);
        }
        else {
            remove_piece(F8); put_piece(BR, H8);
        }
    }
    else if (flags(m) == CASTLE_QUEENSIDE) {
        if (us == WHITE) {
            remove_piece(D1); put_piece(WR, A1);
        }
        else {
            remove_piece(D8); put_piece(BR, A8);
        }
    }

    castling_rights = u.castling_rights;
    fullmove_number = u.fullmove_number;
    fifty_clock = u.fifty_clock;
    en_passant_square = u.ep_square;
    hash = u.hash;
}

void Board::make_null_move(NullUndo& u) {
    u.ep_square = en_passant_square;
    u.hash = hash;

    const Color us = color_to_move;
    color_to_move = us == WHITE ? BLACK : WHITE;
    hash ^= zobrist_side;

    if (en_passant_square != NO_SQUARE) {
        hash ^= zobrist_ep_file[file_of(en_passant_square)];
    }
    en_passant_square = NO_SQUARE;
}

void Board::unmake_null_move(const NullUndo& u) {
    color_to_move = static_cast<Color>(!color_to_move);
    en_passant_square = u.ep_square;
    hash = u.hash;
}

uint64_t Board::compute_hash_from_scratch() const {
    uint64_t h = 0;

    for (int sq = 0; sq < 64; sq++)
        if (mailbox[sq] != NO_PIECE)
            h ^= zobrist_pieces[mailbox[sq]][sq];

    h ^= zobrist_castling[castling_rights];

    if (en_passant_square != NO_SQUARE)
        h ^= zobrist_ep_file[file_of(en_passant_square)];

    if (color_to_move == BLACK)
        h ^= zobrist_side;

    return h;
}

Bitboard Board::attackers_to(const Square square, const Bitboard occupancy) const {
    Bitboard result = 0;

    result |= pawn_attacks[WHITE][square] & piece_bitboards[BP];
    result |= pawn_attacks[BLACK][square] & piece_bitboards[WP];

    result |= knight_attacks[square] & (piece_bitboards[WN] | piece_bitboards[BN]);
    result |= king_attacks[square] & (piece_bitboards[WK] | piece_bitboards[BK]);

    const Bitboard diagonal_attackers = piece_bitboards[WB] | piece_bitboards[WQ] | piece_bitboards[BB] | piece_bitboards[BQ];
    const Bitboard line_attackers = piece_bitboards[WR] | piece_bitboards[WQ] | piece_bitboards[BR] | piece_bitboards[BQ];

    result |= rook_attacks(square, occupancy) & line_attackers;
    result |= bishop_attacks(square, occupancy) & diagonal_attackers;

    return result;
}
