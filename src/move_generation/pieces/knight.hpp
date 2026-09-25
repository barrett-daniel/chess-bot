#pragma once

#include "util/constants.hpp"
#include "move_generation/move.hpp"
#include "util/types.hpp"


void init_knight_attacks();

tracked_array<Move, MAX_NUM_MOVES> get_knight_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces
);
