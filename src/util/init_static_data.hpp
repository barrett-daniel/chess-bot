#pragma once
#include "move_generation/move_generator.hpp"
#include "ai/mvvlva.hpp"
#include "board_Data/zobrist_hashing.hpp"
#include "move_generation/pieces/pieces.hpp"

inline void init_static_data() {
    init_zhashing();
    int_mvvlva_scores();

    init_knight_attacks();
    init_king_attacks();
    init_pawn_attacks(false);
    init_pawn_attacks(true);
    init_magic_bbs();
}
