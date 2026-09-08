#pragma once

#include "../utility/util.hpp"

void init_knight_attacks();

tracked_array<Move, MAX_NUM_MOVES> get_knight_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces
);
