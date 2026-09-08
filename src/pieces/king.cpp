#include "king.hpp"
#include "../utility/logger.hpp"

// TODO: castling

void encode_king_move_data(Bitboard bb, int from_delta, tracked_array<Move, MAX_NUM_MOVES>& moves);

void init_king_attacks() {
    for (Square sq = 0; sq < BOARD_SIZE * BOARD_SIZE; sq++) {
        Bitboard bb = 0ULL;
        Bitboard from_bb = 1ULL << sq;

        for (auto [dir, mask] : KING_DIRECTIONS) {
            bb |= shift_dir(from_bb & ~mask, dir);
        }
        KING_ATTACKS[sq] = bb;
    }
}

tracked_array<Move, MAX_NUM_MOVES> get_king_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    uint8_t castling_rights,
    bool is_white
) {
    tracked_array<Move, MAX_NUM_MOVES> moves;

    for(auto [dir, mask] : KING_DIRECTIONS) {
        Bitboard bb = shift_dir(from_bb & ~mask, dir) & ~own_pieces;
        encode_king_move_data(bb, -dir, moves);
    }

    // castling
    bool can_queen_castle = is_white 
        ? castling_rights & W_QUEEN_CASTLE_MASK 
        : castling_rights & B_QUEEN_CASTLE_MASK;

    bool can_king_castle = is_white 
        ? castling_rights & W_KING_CASTLE_MASK 
        : castling_rights & B_KING_CASTLE_MASK;

    Square castle_from = is_white ? W_KING_START_POS : B_KING_START_POS;

    if(can_queen_castle) {
        Square queen_castle_to = is_white ? W_QUEEN_CASTLE_TO : B_QUEEN_CASTLE_TO;

        Move move(castle_from, queen_castle_to, PieceType::KING);
        move.q_castle = true;

        moves.push_back(move);
    }

    if(can_king_castle) {
        Square king_castle_to = is_white ? W_KING_CASTLE_TO : B_KING_CASTLE_TO;

        Move move(castle_from, king_castle_to, PieceType::KING);
        move.k_castle = true;

        moves.push_back(move);
    }

    return moves;
}

void encode_king_move_data(
    Bitboard bb, 
    int from_delta,
    tracked_array<Move, MAX_NUM_MOVES>& moves
) {
    while(bb) {
        Square to = pop_lsb(bb);
        Square from = to + from_delta;
        moves.emplace_back(from, to, PieceType::KING);
    }
}