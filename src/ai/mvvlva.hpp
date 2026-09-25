#pragma once

#include "move_generation/move.hpp"
#include "board_data/board_state.hpp"

void int_mvvlva_scores();
int get_mvvlva_score(const Move& move, const BoardState& bs);