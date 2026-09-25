#pragma once

#include "util/types.hpp"
#include "util/constants.hpp"
#include "move_generation/move.hpp"
#include "board_data/board_state.hpp"

tracked_array<Move, MAX_NUM_MOVES> get_moves(Colour side, BoardState& bs, int pos);
tracked_array<Move, MAX_NUM_MOVES> get_moves(Colour side, BoardState& bs);
