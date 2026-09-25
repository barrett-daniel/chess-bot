#pragma once

#include <bit>

#include "types.hpp"

constexpr void set_bit(Bitboard& bb, Square pos) {
    bb |= (1ULL << pos);
}

constexpr void clear_bit(Bitboard& bb, Square pos) {
    bb &= ~(1ULL << pos);
}

constexpr bool is_bit(const Bitboard& bb, Square pos) {
    return (bb & (1ULL << pos)) != 0;
}

// pops the LSB of a bitboard -> return index of bit
constexpr Square pop_lsb(Bitboard &bb) {
    Square index = std::countr_zero(bb);
    bb &= bb - 1;
    return index;
}

inline Bitboard shift_dir(Bitboard bb, int dir) {
    return dir > 0 ? bb << dir : bb >> (dir * -1);
};
