#include "knight.hpp"
#include "../utility/logger.hpp"

void encode_knight_move_data(Bitboard bb, int from_delta, std::vector<Move>& moves);

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

std::vector<Move> get_knight_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces
) {
    std::vector<Move> moves;

    for(auto [dir, mask] : KNIGHT_DIRECTIONS) {
        Bitboard bb = shift_dir(from_bb & ~mask, dir) & ~own_pieces;
        encode_knight_move_data(bb, -dir, moves);
    }

    return moves;
}

void encode_knight_move_data(
    Bitboard bb, 
    int from_delta,
    std::vector<Move>& moves
) {
    while(bb) {
        Square to = pop_lsb(bb);
        Square from = to + from_delta;
        moves.emplace_back(from, to, PieceType::KNIGHT);
    }
}