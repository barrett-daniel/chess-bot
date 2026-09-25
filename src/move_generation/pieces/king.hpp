#pragma once
#include "util/constants.hpp"
#include "move_generation/move.hpp"

void init_king_attacks();

tracked_array<Move, MAX_NUM_MOVES> get_king_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    u8 castling_rights,
    bool is_white
);