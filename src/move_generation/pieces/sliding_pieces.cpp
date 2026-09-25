#include "sliding_pieces.hpp"

#include "magic_bitboard.hpp"
#include "util/logger/logger.hpp"
#include "util/bit_utils.hpp"

// ------------------------------------
// Private Function Prototypes
// ------------------------------------

/**
 * Gets the moves for a given sliding piece
 *
 * @param from_bb
 * @param own_pieces_bb
 * @param occupancy_bb
 * @param own_pieces
 * @param enemy_pieces bitboard of enemy
 * @param attack_func the function which defines the attack pattern of the sliding piece
 * @param piece_type the `PieceType` of the sliding piece
 * @return the array of encoded `Move`s
 */
tracked_array<Move, MAX_NUM_MOVES> get_sliding_piece_moves(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb,
    Bitboard (*attack_func)(Square sq, Bitboard all_occupancy),
    PieceType piece_type
);


Bitboard get_sliding_piece_attacks(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb,
    Bitboard (*attack_func)(Square sq, Bitboard all_occupancy)
);

Bitboard get_bishop_attacks(
    Square sq,
    Bitboard all_occupancy
);

Bitboard get_rook_attacks(
    Square sq,
    Bitboard all_occupancy
);

// ------------------------------------
// Public functions
// ------------------------------------


Bitboard get_rook_attack_sqs(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb
) {
    return get_sliding_piece_attacks(from_bb, own_pieces_bb, occupancy_bb, get_rook_attacks);
}

Bitboard get_bishop_attack_sqs(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb
) {
    return get_sliding_piece_attacks(from_bb, own_pieces_bb, occupancy_bb, get_bishop_attacks);
}

tracked_array<Move, MAX_NUM_MOVES> get_rook_moves(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb
) {
    return get_sliding_piece_moves(from_bb, own_pieces_bb, occupancy_bb, get_rook_attacks, PieceType::ROOK);
}

tracked_array<Move, MAX_NUM_MOVES> get_bishop_moves(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb
) {
    return get_sliding_piece_moves(from_bb, own_pieces_bb, occupancy_bb, get_bishop_attacks, PieceType::BISHOP);
}

tracked_array<Move, MAX_NUM_MOVES> get_queen_moves(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb
) {
    auto rook_moves = get_sliding_piece_moves(from_bb, own_pieces_bb, occupancy_bb, get_rook_attacks, PieceType::QUEEN);
    auto bishop_moves = get_sliding_piece_moves(from_bb, own_pieces_bb, occupancy_bb, get_bishop_attacks, PieceType::QUEEN);

    rook_moves.insert(rook_moves.end(), bishop_moves.begin(), bishop_moves.end());
    return rook_moves;
}

// ------------------------------------
// Private Function Definitions
// ------------------------------------

tracked_array<Move, MAX_NUM_MOVES> get_sliding_piece_moves(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb,
    Bitboard (*attack_func)(Square sq, Bitboard all_occupancy),
    PieceType piece_type
) {
    tracked_array<Move, MAX_NUM_MOVES> moves;

    while (from_bb) {
        Square from = pop_lsb(from_bb);
        Bitboard attacks = attack_func(from, occupancy_bb) & ~own_pieces_bb; // strip illegal "captures" of own pieces
        while (attacks) {
            Square to = pop_lsb(attacks);
            moves.emplace_back(from, to, piece_type);
        }
    }
    return moves;
}

Bitboard get_sliding_piece_attacks(
    Bitboard from_bb,
    Bitboard own_pieces_bb,
    Bitboard occupancy_bb,
    Bitboard (*attack_func)(Square sq, Bitboard all_occupancy)
) {
    Bitboard attacks = 0;
    while (from_bb) {
        Square from = pop_lsb(from_bb);
        attacks |= attack_func(from, occupancy_bb) & ~own_pieces_bb; // strip out illegal "captures" of own pieces
    }
    return attacks;
}

Bitboard get_rook_attacks(Square sq, Bitboard all_occupancy) {
    Bitboard blockers = all_occupancy & ROOK_OCCUPANCY_MASK[sq];
    Bitboard idx = (blockers * ROOK_MAGIC[sq]) >> ROOK_SHIFT[sq];
    return ROOK_ATTACKS[sq][idx];
}

Bitboard get_bishop_attacks(Square sq, Bitboard all_occupancy) {
    Bitboard blockers = all_occupancy & BISHOP_OCCUPANCY_MASK[sq];
    Bitboard idx = (blockers * BISHOP_MAGIC[sq]) >> BISHOP_SHIFT[sq];
    return BISHOP_ATTACKS[sq][idx];
}