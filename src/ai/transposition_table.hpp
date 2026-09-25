#pragma once

#include <cstdint>
#include <vector>

#include "../move_generation/move.hpp"

enum class TTFlag : u8 {
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};

struct TTEntry {
    u64 key;
    int score;
    int depth;
    TTFlag flag;
    Move best_move;
};

class TranspositionTable {
    public:
        TranspositionTable();
        TTEntry* at(u64 key);
        void store(u64 key, int score, int depth, TTFlag flag, const Move& best_move);
        void clear();
    private:
        std::vector<TTEntry> table;
        size_t mask; // size is power of 2, index = key & mask
};