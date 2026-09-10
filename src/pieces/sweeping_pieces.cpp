#include "sweeping_pieces.hpp"

#include "magic_bitboard.hpp"
#include "../utility/logger.hpp"

// ------------------------------------
// Private Function Prototypes
// ------------------------------------

tracked_array<Move, MAX_NUM_MOVES> get_sweep_moves_magic(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces,
    Bitboard (*attack_func)(Square sq, Bitboard all_occupancy),
    PieceType piece_type
);


Bitboard get_sliding_piece_attacks(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces,
    Bitboard (*attack_func)(Square sq, Bitboard all_occupancy)
);

Bitboard bishop_attacks(
    Square sq,
    Bitboard all_occupancy
);

Bitboard rook_attacks(
    Square sq,
    Bitboard all_occupancy
);

// ------------------------------------
// Public functions
// ------------------------------------


Bitboard get_rook_attack_sqs(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    return get_sliding_piece_attacks(from_bb, own_pieces, enemy_pieces, rook_attacks);
}

Bitboard get_bishop_attack_sqs(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    return get_sliding_piece_attacks(from_bb, own_pieces, enemy_pieces, bishop_attacks);
}

tracked_array<Move, MAX_NUM_MOVES> get_rook_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    return get_sweep_moves_magic(from_bb, own_pieces, enemy_pieces, rook_attacks, ROOK);
}

tracked_array<Move, MAX_NUM_MOVES> get_bishop_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    return get_sweep_moves_magic(from_bb, own_pieces, enemy_pieces, bishop_attacks, BISHOP);
}

tracked_array<Move, MAX_NUM_MOVES> get_queen_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    auto rook_moves = get_sweep_moves_magic(from_bb, own_pieces, enemy_pieces, rook_attacks, QUEEN);
    auto bishop_moves = get_sweep_moves_magic(from_bb, own_pieces, enemy_pieces, bishop_attacks, QUEEN);

    rook_moves.insert(rook_moves.end(), bishop_moves.begin(), bishop_moves.end());
    return rook_moves;
}

// ------------------------------------
// Private Function Definitions
// ------------------------------------

tracked_array<Move, MAX_NUM_MOVES> get_sweep_moves_magic(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces,
    Bitboard (*attack_func)(Square sq, Bitboard all_occupancy),
    PieceType piece_type
) {
    tracked_array<Move, MAX_NUM_MOVES> moves;
    Bitboard occ = own_pieces | enemy_pieces;

    while (from_bb) {
        Square from = pop_lsb(from_bb);
        Bitboard attacks = attack_func(from, occ) & ~own_pieces; // strip illegal "captures" of own pieces
        while (attacks) {
            Square to = pop_lsb(attacks);
            moves.emplace_back(from, to, piece_type);
        }
    }
    return moves;
}

Bitboard get_sliding_piece_attacks(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces,
    Bitboard (*attack_func)(Square sq, Bitboard all_occupancy)
) {
    Bitboard occ = own_pieces | enemy_pieces;
    Bitboard attacks = 0;
    while (from_bb) {
        Square from = pop_lsb(from_bb);
        attacks |= attack_func(from, occ) & ~own_pieces; // strip out illegal "captures" of own pieces
    }
    return attacks;
}

Bitboard rook_attacks(Square sq, Bitboard all_occupancy) {
    Bitboard blockers = all_occupancy & ROOK_OCCUPANCY_MASK[sq];
    Bitboard idx = (blockers * ROOK_MAGIC[sq]) >> ROOK_SHIFT[sq];
    return ROOK_ATTACKS[sq][idx];
}

Bitboard bishop_attacks(Square sq, Bitboard all_occupancy) {
    Bitboard blockers = all_occupancy & BISHOP_OCCUPANCY_MASK[sq];
    Bitboard idx = (blockers * BISHOP_MAGIC[sq]) >> BISHOP_SHIFT[sq];
    return BISHOP_ATTACKS[sq][idx];
}