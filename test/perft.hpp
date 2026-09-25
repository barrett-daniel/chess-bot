#pragma once
#include <doctest/doctest.h>

#include "board_data/board_manager.hpp"

/**
 * Performs a PERFT test (as defined here: https://chessprogramming.org/Perft) counting the
 * total number nodes in the PERFT-tree for the given depth.
 *
 * i.e. counts the total number of possible positions from the given position and depth.
 */
inline u64 perft(BoardManager& bm, int depth) {
    tracked_array<Move, MAX_NUM_MOVES> moves = get_moves(bm.get_bs().turn, bm.get_bs());
    tracked_array<Move, MAX_NUM_MOVES> legal_moves = bm.get_legal_moves(moves);

    if (depth == 1) {
        return legal_moves.size();
    }

    u64 nodes = 0;
    for (const Move& move : legal_moves) {
        bm.make_move(move);
        nodes += perft(bm, depth - 1);
        bm.undo_move();
    }
    return nodes;
}

/**
 * Performs PERFT up to the given depth, counting the number of resulting nodes in the PERFT-trees
 * generated from each possible move at the current state. Logs a message for the number of nodes
 */
inline void divide(BoardManager& bm, int depth) {
    tracked_array<Move, MAX_NUM_MOVES> moves = get_moves(bm.get_bs().turn, bm.get_bs());
    tracked_array<Move, MAX_NUM_MOVES> legal_moves = bm.get_legal_moves(moves);

    u64 total = 0;
    for (const Move& move : legal_moves) {
        bm.make_move(move);
        u64 num = (depth == 1) ? 1 : perft(bm, depth - 1);
        bm.undo_move();

        MESSAGE(move.to_string() << ": " << num);
        total += num;
    }
    MESSAGE("Total: " << total);
}