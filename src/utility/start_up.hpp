#pragma once
#include "delegates/move_generator.hpp"
#include "../delegates/ai/mvvlva.hpp"
#include "utility/zobrist_hashing.hpp"
#include "pieces/pieces.hpp"

inline void init_static_data() {
    init_zhashing();
    int_mvvlva_scores();

    init_knight_attacks();
    init_king_attacks();
    init_pawn_attacks(false);
    init_pawn_attacks(true);
    init_magic_bbs();
}
