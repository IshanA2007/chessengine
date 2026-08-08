//
// Created by Ishan Ajwani on 8/8/26.
//

#include "board.h"
#include <iostream>
#include <sstream>

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
            file++;
        }
    }
}

void Board::print() {
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


