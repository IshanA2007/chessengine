//
// Created by Ishan Ajwani on 8/9/26.
//

#include "attacks.h"

#include <iostream>

Bitboard rook_table[64][4096]{};
Bitboard bishop_table[64][512]{};
Bitboard rook_masks[64]{};
Bitboard bishop_masks[64]{};
int rook_shifts[64]{};
int bishop_shifts[64]{};

Bitboard slow_rook_attacks(Square rook_square, Bitboard occupations) {
    Bitboard attacks = 0;

    constexpr std::array<std::pair<int, int>, 4> rook_dirs = {{
        {1, 0}, {0, 1}, {-1, 0}, {0, -1}
    }};
    const int rank = rook_square / 8;
    const int file = rook_square % 8;
    for (auto [dr, df] : rook_dirs) {
        int r = rank + dr;
        int f = file + df;
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            const int square = f + 8 * r;
            attacks |= 1ULL << square;
            //inc
            if ((1ULL << square) & occupations)  {
                break;
            }
            r += dr;
            f += df;
        }
    }


    return attacks;
}

Bitboard slow_bishop_attacks(Square bishop_square, Bitboard occupations) {
    Bitboard attacks = 0;

    constexpr std::array<std::pair<int, int>, 4> bishop_dirs = {{
        {-1, -1}, {1, 1}, {-1, 1}, {1, -1}
    }};
    const int rank = bishop_square / 8;
    const int file = bishop_square % 8;
    for (auto [dr, df] : bishop_dirs) {
        int r = rank + dr;
        int f = file + df;
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            const int square = f + 8 * r;
            attacks |= 1ULL << square;
            if ((1ULL << square) & occupations)  {
                break;
            }
            //inc
            r += dr;
            f += df;
        }
    }


    return attacks;
}

Bitboard relevant_occupancy_rook_mask(Square rook_square) {
    Bitboard attacks = 0;

    constexpr std::array<std::pair<int, int>, 4> rook_dirs = {{
        {1, 0}, {0, 1}, {-1, 0}, {0, -1}
    }};
    const int rank = rook_square / 8;
    const int file = rook_square % 8;
    for (auto [dr, df] : rook_dirs) {
        int r = rank + dr;
        int f = file + df;
        const int upper_rank_lim = (rank == 7) ? 8 : 7;
        const int lower_rank_lim = (rank == 0) ? 0 : 1;
        const int upper_file_lim = (file == 7) ? 8 : 7;
        const int lower_file_lim = (file == 0) ? 0 : 1;

        while (r >= lower_rank_lim && r < upper_rank_lim && f >= lower_file_lim && f < upper_file_lim) {
            const int square = f + 8 * r;
            attacks |= 1ULL << square;
            //inc
            r += dr;
            f += df;
        }
    }


    return attacks;
}

Bitboard relevant_occupancy_bishop_mask(Square bishop_square) {
    Bitboard attacks = 0;

    constexpr std::array<std::pair<int, int>, 4> bishop_dirs = {{
        {-1, -1}, {1, 1}, {-1, 1}, {1, -1}
    }};
    const int rank = bishop_square / 8;
    const int file = bishop_square % 8;
    for (auto [dr, df] : bishop_dirs) {
        int r = rank + dr;
        int f = file + df;
        const int upper_rank_lim = (rank == 7) ? 8 : 7;
        const int lower_rank_lim = (rank == 0) ? 0 : 1;
        const int upper_file_lim = (file == 7) ? 8 : 7;
        const int lower_file_lim = (file == 0) ? 0 : 1;

        while (r >= lower_rank_lim && r < upper_rank_lim && f >= lower_file_lim && f < upper_file_lim) {
            const int square = f + 8 * r;
            attacks |= 1ULL << square;
            //inc
            r += dr;
            f += df;
        }
    }


    return attacks;
}


void init_magics() {
    for (int i = 0; i < 64; i++) {
        Bitboard rmask = relevant_occupancy_rook_mask(static_cast<Square>(i));
        Bitboard bmask = relevant_occupancy_bishop_mask(static_cast<Square>(i));

        int rook_num_relevant_bits = std::popcount(rmask);
        int bishop_num_relevant_bits = std::popcount(bmask);

        rook_masks[i] = rmask;
        rook_shifts[i] = 64 - rook_num_relevant_bits;

        bishop_masks[i] = bmask;
        bishop_shifts[i] = 64 - bishop_num_relevant_bits;

        Bitboard rsubset = 0;
        Bitboard bsubset = 0;

        do {
            int rindex = (rsubset * rook_magics[i]) >> rook_shifts[i];
            Bitboard rattacks = slow_rook_attacks(static_cast<Square>(i), rsubset);
            if (rook_table[i][rindex] != 0 && rook_table[i][rindex] != rattacks){
                std::cout << "BIG CRASH" << std::endl;
                exit(1);
            }
            rook_table[i][rindex] = rattacks;
            rsubset = (rsubset - rmask) & rmask;
        } while (rsubset);

        do {
            int bindex = (bsubset * bishop_magics[i]) >> bishop_shifts[i];
            Bitboard battacks = slow_bishop_attacks(static_cast<Square>(i), bsubset);
            if (bishop_table[i][bindex] != 0 && bishop_table[i][bindex] != battacks) {
                std::cout << "BIG CRASH (bishop)" << std::endl;
                exit(1);
            }
            bishop_table[i][bindex] = battacks;
            bsubset = (bsubset - bmask) & bmask;
        } while (bsubset);

    }
}