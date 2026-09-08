#pragma once

#include "../utility/util.hpp"

void init_pawn_attacks(bool is_white);

Bitboard get_pawn_attack_sqs(
    Bitboard from_bb,
    bool is_white
);

std::vector<Move> get_pawn_moves(
    Bitboard from_bb,
    bool is_white,
    const Bitboard& enemy_bb,
    const Bitboard& all_bb,
    Square passant_sq
);