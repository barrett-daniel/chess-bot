//
// Created by dan12 on 10/09/2026.
//


#include <cassert>
#include <random>
#include <doctest/doctest.h>

#include "pieces/magic_bitboard.hpp"
#include "utility/util.hpp"

uint64_t rand_u64() {
    static std::mt19937_64 rng(12345);
    return rng();
}

TEST_SUITE("Magic Tests") {
    TEST_CASE("Rook Magic") {
        init_magic_bbs();

        for (Square sq = 0; sq < 64; sq++) {
            for (int trial = 0; trial < 100000; trial++) {
                Bitboard occ = rand_u64() & ROOK_OCCUPANCY_MASK[sq];
                Bitboard idx = (occ * ROOK_MAGIC[sq]) >> ROOK_SHIFT[sq];
                CHECK_EQ(ROOK_ATTACKS[sq][idx], rook_attacks_from_blockers(sq, occ));
            }
        }
    }

    TEST_CASE("Bishop Magic") {
        init_magic_bbs();

        for (Square sq = 0; sq < 64; sq++) {
            for (int trial = 0; trial < 100000; trial++) {
                Bitboard occ = rand_u64() & BISHOP_OCCUPANCY_MASK[sq];
                Bitboard idx = (occ * BISHOP_MAGIC[sq]) >> BISHOP_SHIFT[sq];
                CHECK_EQ(BISHOP_ATTACKS[sq][idx], bishop_attacks_from_blockers(sq, occ));
            }
        }
    }
}