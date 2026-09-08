#pragma once

#include "../utility/util.hpp"

std::vector<Move> get_moves(Colour side, BoardState& bs, int pos);
std::vector<Move> get_moves(Colour side, BoardState& bs);

void initalise_attack_arrays();
