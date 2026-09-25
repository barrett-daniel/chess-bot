#pragma once
#include "util/constants.hpp"
#include "move_generation/move.hpp"

Bitboard get_rook_attack_sqs(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb
);

Bitboard get_bishop_attack_sqs(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb
);

tracked_array<Move, MAX_NUM_MOVES> get_rook_moves(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb
);

tracked_array<Move, MAX_NUM_MOVES> get_bishop_moves(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb
);

tracked_array<Move, MAX_NUM_MOVES> get_queen_moves(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb
);

