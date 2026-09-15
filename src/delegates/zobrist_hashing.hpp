#pragma once
#include "utility/util.hpp"

// below are a series of arrays of random numbers which are the hash keys
// the hash value is calculated by indexing into the these array using the
// board state data and XORing the hash with itself to build a cumulative hash.

// One key per (colour, piece type, square).
extern uint64_t ZPIECE_SQ[NUM_COLOURS][NUM_PIECE_TYPES][NUM_SQUARES];

// NUM_CASTLING_STATES possible castling states (as its encoded as 4 bits and 2^4 = NUM_CASTLING_STATES)
// -> have a random number for each state, index into array for the random key
constexpr uint8_t NUM_CASTLING_STATES = 16;
extern uint64_t ZCASTLING[NUM_CASTLING_STATES];

// En passant is only relevant by file (the rank is implied by whose
// turn it is)
extern uint64_t ZEP_FILE[BOARD_SIZE];

// XORed in only when it's Black to move.
extern uint64_t ZSIDE;

void init_zhashing();
uint64_t zhash(const BoardState& bs);