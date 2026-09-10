#pragma once

#include "../utility/util.hpp"

Bitboard get_rook_attack_sqs(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
);

Bitboard get_bishop_attack_sqs(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
);

tracked_array<Move, MAX_NUM_MOVES> get_rook_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
);

tracked_array<Move, MAX_NUM_MOVES> get_bishop_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
);

tracked_array<Move, MAX_NUM_MOVES> get_queen_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
);

