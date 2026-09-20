#pragma once

#include "../utility/util.hpp"

tracked_array<Move, MAX_NUM_MOVES> get_moves(Colour side, BoardState& bs, int pos);
tracked_array<Move, MAX_NUM_MOVES> get_moves(Colour side, BoardState& bs);
