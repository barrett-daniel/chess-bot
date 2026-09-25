#include "move_generation/pieces/pawn.hpp"
#include "util/logger/logger.hpp"
#include "util/bit_utils.hpp"

#define PASSANT 1
#define PROMOTION 1 << 1

void append_promotions(tracked_array<Move, MAX_NUM_MOVES>& moves, const Move& base);
void encode_pawn_move_data(Bitboard bb, int from_delta, tracked_array<Move, MAX_NUM_MOVES>& moves, long flags);

inline Bitboard shift_fwd(bool is_white, Bitboard bb, u8 val) {
    if(is_white) return bb << val;

    return bb >> val;
}

inline void get_diag_shifts(bool is_white, u8 *l, u8 *r) {
    *l = BOARD_SIZE - (is_white ? 1 : -1);
    *r = BOARD_SIZE + (is_white ? 1 : -1);
}

void init_pawn_attacks(bool is_white) {
    u8 diag_l_shift, diag_r_shift;
    get_diag_shifts(is_white, &diag_l_shift, &diag_r_shift);

    // pre-computing what squares (if occupied by pawns) could attack each square index is equivalent to computing what
    // squares a pawn could attack from the given square index AND THEN inverting the colour for which such was calculated
    //
    // i.e. from a given square if a white pawn can attack the two squares diagonally infront of it, if there were
    // two black pawns in those squares, either could attack the white pawn.
    for (Square sq = 0; sq < 64; sq++) {
        Bitboard from_bb = 1ULL << sq;

        // calculate what squares a pawn at that index could attack
        Bitboard captures_l = shift_fwd(is_white, from_bb, diag_l_shift) & ~FILE_H;
        Bitboard captures_r = shift_fwd(is_white, from_bb, diag_r_shift) & ~FILE_A;
        Bitboard attacks = captures_l | captures_r;

        // store those attack squares in the reverse colour pre-computed attack array
        is_white ? PAWN_ATTACKS_B[sq] = attacks : PAWN_ATTACKS_W[sq] = attacks;
    }
}

tracked_array<Move, MAX_NUM_MOVES> get_pawn_moves(
    Bitboard from_bb,
    bool is_white,
    const Bitboard& enemy_bb,
    const Bitboard& all_bb,
    Square passant_sq
) {
    u8 diag_l_shift, diag_r_shift;
    get_diag_shifts(is_white, &diag_l_shift, &diag_r_shift);
    tracked_array<Move, MAX_NUM_MOVES> moves;

    Bitboard prom_rank = is_white ? RANK_8 : RANK_1;
    Bitboard empty = ~all_bb;

    // construct bitboards for single and double pushes
    Bitboard singles = shift_fwd(is_white, from_bb, BOARD_SIZE) & empty;
    Bitboard double_push_rank = is_white ? RANK_3 : RANK_6;
    Bitboard doubles = shift_fwd(is_white, singles & double_push_rank, BOARD_SIZE) & empty;

    // construct bitboards for diagonal moves i.e. en-passant and captures
    Bitboard passant_bb = 0;
    if(passant_sq != NULL_SQ) set_bit(passant_bb, passant_sq);

    Bitboard captures_l = shift_fwd(is_white, from_bb, diag_l_shift) & enemy_bb & ~FILE_H;
    Bitboard passant_l = shift_fwd(is_white, from_bb, diag_l_shift) & passant_bb & ~FILE_H & ~double_push_rank;

    Bitboard captures_r = shift_fwd(is_white, from_bb, diag_r_shift) & enemy_bb & ~FILE_A;
    Bitboard passant_r = shift_fwd(is_white, from_bb, diag_r_shift) & passant_bb & ~FILE_A & ~double_push_rank;

    // filter out any previous moves if they are also a promotion and store these
    // in an associated promotion bitboard
    Bitboard prom_pushes = singles & prom_rank;
    singles &= ~prom_rank;

    Bitboard prom_captures_l = captures_l & prom_rank;
    captures_l &= ~prom_rank;

    Bitboard prom_captures_r= captures_r & prom_rank;
    captures_r &= ~prom_rank;

    // embed bitboards into Move structures
    int forward_d = is_white ? BOARD_SIZE : -BOARD_SIZE;
    int double_forward_d = is_white ? BOARD_SIZE * 2 : -BOARD_SIZE * 2;
    int left_diag_d = is_white ? diag_l_shift : -diag_l_shift;
    int right_diag_d = is_white ? diag_r_shift : -diag_r_shift;

    encode_pawn_move_data(singles, forward_d, moves, 0);
    encode_pawn_move_data(doubles, double_forward_d, moves, 0);
    encode_pawn_move_data(captures_l, left_diag_d, moves, 0);
    encode_pawn_move_data(captures_r, right_diag_d, moves, 0);

    encode_pawn_move_data(passant_l, left_diag_d, moves, PASSANT);
    encode_pawn_move_data(passant_r, right_diag_d, moves, PASSANT);

    encode_pawn_move_data(prom_pushes, forward_d, moves, PROMOTION);
    encode_pawn_move_data(prom_captures_l, left_diag_d, moves, PROMOTION);
    encode_pawn_move_data(prom_captures_r, right_diag_d, moves, PROMOTION);

    return moves;
}

// appends all promotion variants of a given base move to a list of moves
void append_promotions(tracked_array<Move, MAX_NUM_MOVES>& moves, const Move& base) {
    for (PieceType type : {PieceType::QUEEN, PieceType::ROOK, PieceType::BISHOP, PieceType::KNIGHT}) {
        Move promoted = base;
        promoted.promotion_piece = type;
        moves.push_back(promoted);
    }
}

// encodes the move data: Bitboard -> Move
// 'from' locations are calculated retroactively from each bit position
// in the bb based upon the from_delta
void encode_pawn_move_data(
    Bitboard bb, 
    int from_delta, 
    tracked_array<Move, MAX_NUM_MOVES>& moves, 
    long flags
) {
    while(bb) {
        Square to = pop_lsb(bb);
        Square from = to - from_delta;

        Move move(from, to, PieceType::PAWN);
        move.is_en_passant = flags & PASSANT;

        (flags & PROMOTION) ? append_promotions(moves, move) : moves.push_back(move);
    }
}

