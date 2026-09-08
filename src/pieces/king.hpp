#pragma once

#include "../utility/util.hpp"

void init_king_attacks();

tracked_array<Move, MAX_NUM_MOVES> get_king_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    uint8_t castling_rights,
    bool is_white
);