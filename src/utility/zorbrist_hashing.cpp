#include "zobrist_hashing.hpp"

#include <random>

uint64_t ZPIECE_SQ[NUM_COLOURS][NUM_PIECE_TYPES][NUM_SQUARES] = {};
uint64_t ZCASTLING[NUM_CASTLING_STATES] = {};
uint64_t ZEP_FILE[BOARD_SIZE] = {};
uint64_t ZSIDE = 0;

constexpr uint64_t SEED = 0xABCDEF1234;

void init_zhashing() {
    std::mt19937_64 rng(SEED);
    std::uniform_int_distribution<uint64_t> dist;

    for (uint8_t c = 0; c < NUM_COLOURS; ++c) {
        for (uint8_t pt = 0; pt < NUM_PIECE_TYPES; ++pt) {
            for (uint8_t sq = 0; sq < NUM_SQUARES; ++sq) {
                ZPIECE_SQ[c][pt][sq] = dist(rng);
            }
        }
    }

    for (int i = 0; i < NUM_CASTLING_STATES; ++i) ZCASTLING[i] = dist(rng);
    for (int f = 0; f < BOARD_SIZE; ++f) ZEP_FILE[f] = dist(rng);
    ZSIDE = dist(rng);
}

uint64_t zhash(const BoardState& bs) {
    uint64_t h = 0;

    // piece hash
    for (uint8_t c = 0; c < NUM_COLOURS; ++c) {
        Colour colour = static_cast<Colour>(c);

        for (uint8_t pt = 0; pt < NUM_PIECE_TYPES; ++pt) {
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
        uint8_t file = static_cast<uint8_t>(bs.en_passant_square) & (BOARD_SIZE - 1);
        h ^= ZEP_FILE[file];
    }

    // turn hash
    if (bs.turn == Colour::BLACK) h ^= ZSIDE;

    return h;
}