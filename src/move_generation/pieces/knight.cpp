#include "knight.hpp"
#include "util/bit_utils.hpp"

void init_knight_attacks() {
    for (Square sq = 0; sq < BOARD_SIZE * BOARD_SIZE; sq++) {
        Bitboard bb = 0ULL;
        Bitboard from_bb = 1ULL << sq;

        for (auto [dir, mask] : KNIGHT_DIRECTIONS) {
            bb |= shift_dir(from_bb & ~mask, dir);
        }
        KNIGHT_ATTACKS[sq] = bb;
    }
}

tracked_array<Move, MAX_NUM_MOVES> get_knight_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces
) {
    tracked_array<Move, MAX_NUM_MOVES> moves;

    while (from_bb) {
        Square from = pop_lsb(from_bb);
        Bitboard targets = KNIGHT_ATTACKS[from] & ~own_pieces;
        while (targets) {
            Square to = pop_lsb(targets);
            moves.emplace_back(from, to, PieceType::KNIGHT);
        }
    }

    return moves;
}