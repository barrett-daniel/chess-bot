#pragma once

#include "utility/util.hpp"

enum class TTFlag : uint8_t {
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};

struct TTEntry {
    uint64_t key;
    int score;
    int depth;
    TTFlag flag;
    Move best_move;
};

class TranspositionTable {
    public:
        TranspositionTable();
        TTEntry* at(uint64_t key);
        void store(uint64_t key, int score, int depth, TTFlag flag, const Move& best_move);
        void clear();
    private:
        std::vector<TTEntry> table;
        size_t mask; // size is power of 2, index = key & mask
};