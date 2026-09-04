#pragma once

#include "../utility/util.hpp"

void init_knight_attacks();

std::vector<Move> get_knight_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces
);
