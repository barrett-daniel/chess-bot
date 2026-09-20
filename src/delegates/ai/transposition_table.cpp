#include "transposition_table.hpp"

#include <algorithm>

constexpr int TT_SIZE_MB = 4;

TranspositionTable::TranspositionTable(){
    size_t num_entries = (TT_SIZE_MB * 1024 * 1024) / sizeof(TTEntry);
    size_t power_of_two = 1;
    while (power_of_two * 2 <= num_entries) power_of_two *= 2;

    table.resize(power_of_two);
    mask = power_of_two - 1;
}

TTEntry* TranspositionTable::at(uint64_t key) {
    TTEntry& e = table[key & mask];
    return (e.key == key) ? &e : nullptr;
}

void TranspositionTable::store(uint64_t key, int score, int depth, TTFlag flag, const Move& best_move) {
    TTEntry& e = table[key & mask];
    // depth-preferred replacement: don't overwrite deeper/newer entries with shallow ones
    if (e.key != key || depth >= e.depth) {
        e = { key, score, depth, flag, best_move};
    }
}

void TranspositionTable::clear() {
    std::ranges::fill(table, TTEntry{});
}
