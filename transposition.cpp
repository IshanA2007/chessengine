//
// Created by Ishan Ajwani on 8/11/26.
//

#include "transposition.h"
#include <vector>
#include <algorithm>

constexpr size_t TT_ENTRIES = 1ULL << 22; // 64 MB

static std::vector<TTEntry> table;

void tt_init() {
    table.assign(TT_ENTRIES, TTEntry{});
}

void tt_clear() {
    std::ranges::fill(table, TTEntry{});
}

static uint64_t slot(const uint64_t hash) {
    return hash & (TT_ENTRIES - 1);
}

bool tt_probe(const uint64_t hash, const int depth, const int alpha, const int beta, int &out_score, Move &out_move) {
    const TTEntry entry = table[slot(hash)];
    out_move = NO_MOVE;

    if (entry.key != hash) {
        return false; //this position isn't stored or relevant
    }

    out_move = entry.best_move; //position is stored, so we have a best_move no matter what

    if (entry.depth < depth) {
        //insufficient depth, exit with just best_move (ordering reccommendation)
        return false;
    }

    if (entry.bound == BOUND_EXACT) {
        out_score = entry.score;
        return true;
    }

    if (entry.bound == BOUND_LOWER && entry.score >= beta) {
        out_score = entry.score;
        return true;
    }

    if (entry.bound == BOUND_UPPER && entry.score <= alpha) {
        out_score = entry.score;
        return true;
    }

    return false;
}

void tt_store(const uint64_t hash, const int depth, const int score, const Move best_move, const int alpha_original, const int beta) {
    Bound bound;
    if (score >= beta) {
        bound = BOUND_LOWER;
    }
    else if (score <= alpha_original) {
        bound = BOUND_UPPER;
    }
    else {
        bound = BOUND_EXACT;
    }

    TTEntry entry = table[slot(hash)];
    entry.key = hash;
    entry.bound = bound;
    entry.depth = depth;
    entry.score = score;
    entry.best_move = best_move;
    table[slot(hash)] = entry;
}


