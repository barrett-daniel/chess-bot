#include "zobrist_hashing.hpp"

#include <random>

#include "util/bit_utils.hpp"

u64 ZPIECE_SQ[NUM_COLOURS][NUM_PIECE_TYPES][NUM_SQUARES] = {};
u64 ZCASTLING[NUM_CASTLING_STATES] = {};
u64 ZEP_FILE[BOARD_SIZE] = {};
u64 ZSIDE = 0;

constexpr u64 SEED = 0xABCDEF1234;

void init_zhashing() {
    std::mt19937_64 rng(SEED);
    std::uniform_int_distribution<u64> dist;

    for (u8 c = 0; c < NUM_COLOURS; ++c) {
        for (u8 pt = 0; pt < NUM_PIECE_TYPES; ++pt) {
            for (u8 sq = 0; sq < NUM_SQUARES; ++sq) {
                ZPIECE_SQ[c][pt][sq] = dist(rng);
            }
        }
    }

    for (int i = 0; i < NUM_CASTLING_STATES; ++i) ZCASTLING[i] = dist(rng);
    for (int f = 0; f < BOARD_SIZE; ++f) ZEP_FILE[f] = dist(rng);
    ZSIDE = dist(rng);
}

u64 zhash(const BoardState& bs) {
    u64 h = 0;

    // piece hash
    for (u8 c = 0; c < NUM_COLOURS; ++c) {
        Colour colour = static_cast<Colour>(c);

        for (u8 pt = 0; pt < NUM_PIECE_TYPES; ++pt) {
            PieceType piece = static_cast<PieceType>(pt);
            Bitboard bb = bs.get_bb(colour, piece);

            while (bb) {
                Square sq = pop_lsb(bb);
                h ^= ZPIECE_SQ[c][pt][sq];
            }
        }
    }

    // castling hash
    h ^= ZCASTLING[bs.castling_rights & 0xF];

    // enpassant hash
    if (bs.en_passant_square != NULL_SQ) {
        u8 file = static_cast<u8>(bs.en_passant_square) & (BOARD_SIZE - 1);
        h ^= ZEP_FILE[file];
    }

    // turn hash
    if (bs.turn == Colour::BLACK) h ^= ZSIDE;

    return h;
}