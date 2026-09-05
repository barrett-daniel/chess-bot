#include "../pieces/pawn.hpp"
#include "../utility/logger.hpp"

#define PASSANT 1
#define PROMOTION 1 << 1

void append_promotions(std::vector<Move>& moves, const Move& base);
void encode_pawn_move_data(Bitboard bb, int from_delta, std::vector<Move>& moves, long flags);

inline Bitboard shift_fwd(bool is_white, Bitboard bb, uint8_t val) { 
    if(is_white) return bb >> val;

    return bb << val;
}

inline void get_diag_shifts(bool is_white, uint8_t *l, uint8_t *r) {
    *l = BOARD_SIZE + (is_white ? 1 : -1);
    *r = BOARD_SIZE - (is_white ? 1 : -1);
}

void init_pawn_attacks(bool is_white) {
    uint8_t diag_l_shift, diag_r_shift;
    get_diag_shifts(is_white, &diag_l_shift, &diag_r_shift);

    for (int sq = 0; sq < 64; sq++) {
        Bitboard from_bb = 1ULL << sq;

        Bitboard captures_l = shift_fwd(is_white, from_bb, diag_l_shift) & ~FILE_H;
        Bitboard captures_r = shift_fwd(is_white, from_bb, diag_r_shift) & ~FILE_A;
        Bitboard attacks = captures_l | captures_r;

        is_white ? PAWN_ATTACKS_W[sq] = attacks : PAWN_ATTACKS_B[sq] = attacks;
    }
}

std::vector<Move> get_pawn_moves(
    Bitboard from_bb,
    bool is_white,
    const Bitboard& enemy_bb,
    const Bitboard& all_bb,
    int passant_sq
) {
    uint8_t diag_l_shift, diag_r_shift;
    get_diag_shifts(is_white, &diag_l_shift, &diag_r_shift);
    std::vector<Move> moves;

    Bitboard prom_rank = is_white ? RANK_1 : RANK_8;
    Bitboard empty = ~all_bb;

    // construct bitboards for single and double pushes
    Bitboard singles = shift_fwd(is_white, from_bb, BOARD_SIZE) & empty;
    Bitboard double_push_rank = is_white ? RANK_6 : RANK_3;
    Bitboard doubles = shift_fwd(is_white, singles & double_push_rank, BOARD_SIZE) & empty;

    // construct bitboards for diagonal moves i.e. en-passant and captures
    Bitboard passant_bb = 0;
    if(passant_sq != -1) set_bit(passant_bb, passant_sq);

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

    encode_pawn_move_data(singles, forward_d, moves, NULL);
    encode_pawn_move_data(doubles, double_forward_d, moves, NULL);
    encode_pawn_move_data(captures_l, left_diag_d, moves, NULL);
    encode_pawn_move_data(captures_r, right_diag_d, moves, NULL);

    encode_pawn_move_data(passant_l, left_diag_d, moves, PASSANT);
    encode_pawn_move_data(passant_r, right_diag_d, moves, PASSANT);

    encode_pawn_move_data(prom_pushes, forward_d, moves, PROMOTION);
    encode_pawn_move_data(prom_captures_l, left_diag_d, moves, PROMOTION);
    encode_pawn_move_data(prom_captures_r, right_diag_d, moves, PROMOTION);

    return moves;
}

// appends all promotion variants of a given base move to a list of moves
void append_promotions(std::vector<Move>& moves, const Move& base) {
    for (PieceType type : {QUEEN, ROOK, BISHOP, KNIGHT}) {
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
    std::vector<Move>& moves, 
    long flags
) {
    while(bb) {
        int to = pop_lsb(bb);
        int from = to + from_delta;

        Move move(from, to, PieceType::PAWN);
        move.is_en_passant = flags & PASSANT;

        (flags & PROMOTION) ? append_promotions(moves, move) : moves.push_back(move);
    }
}

