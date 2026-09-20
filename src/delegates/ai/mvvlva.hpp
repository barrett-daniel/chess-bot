#pragma once

#include "utility/util.hpp"

constexpr uint16_t PAWN_VALUE = 100;
constexpr uint16_t KNIGHT_VALUE = 300;
constexpr uint16_t BISHOP_VALUE = 320;
constexpr uint16_t ROOK_VALUE = 500;
constexpr uint16_t QUEEN_VALUE = 900;

void int_mvvlva_scores();
int get_mvvlva_score(const Move& move, const BoardState& bs);