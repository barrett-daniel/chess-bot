#include "mvvlva.hpp"

#include "utility/util.hpp"

/*************************************************

                 Declarations

*************************************************/

static int MVV_LVA_SCORES[NUM_PIECE_TYPES][NUM_PIECE_TYPES]; // indexed [attacker][victim]

uint16_t score_piece(PieceType pt);

/*************************************************

                Public Functions

*************************************************/

void int_mvvlva_scores() {
    auto score_piece = [](PieceType pt) {
        switch (pt) {
            case PieceType::PAWN: return PAWN_VALUE;
            case PieceType::KNIGHT: return KNIGHT_VALUE;
            case PieceType::BISHOP: return BISHOP_VALUE;
            case PieceType::ROOK: return ROOK_VALUE;
            case PieceType::QUEEN: return QUEEN_VALUE;
            case PieceType::KING: return static_cast<uint16_t>(10'000);
            default: throw std::runtime_error("Unknown PieceType");
        }
    };

    // init MVV_LVA_SCORES
    for (int i = 0; i < NUM_PIECE_TYPES; i++) {
        for (int j = 0; j < NUM_PIECE_TYPES; j++) {
            auto attacker = static_cast<PieceType>(i);
            auto victim = static_cast<PieceType>(j);
            MVV_LVA_SCORES[i][j] = score_piece(victim) - score_piece(attacker);
        }
    }
}

int get_mvvlva_score(const Move& move, const BoardState& bs) {
    int attacker_idx = -1;
    for (int i = 0; i < NUM_PIECE_TYPES; i++) {
        const Bitboard& bb = bs.get_bb(bs.turn, static_cast<PieceType>(i));
        if (is_bit(bb, move.from)) attacker_idx = i;
    }

    int victim_idx = -1;
    for (int i = 0; i < NUM_PIECE_TYPES; i++) {
        const Bitboard& bb = bs.get_bb(!bs.turn, static_cast<PieceType>(i));
        if (is_bit(bb, move.to)) victim_idx = i;
    }

    ASSERT(attacker_idx >= 0 && attacker_idx <= NUM_PIECE_TYPES, "Invalid attacker index");
    ASSERT(victim_idx >= 0 && victim_idx <= NUM_PIECE_TYPES, "Invalid victim index");

    return MVV_LVA_SCORES[attacker_idx][victim_idx];
}

/*************************************************

                Private Functions

*************************************************/

uint16_t score_piece(PieceType pt) {
    switch (pt) {
        case PieceType::PAWN: return PAWN_VALUE;
        case PieceType::KNIGHT: return KNIGHT_VALUE;
        case PieceType::BISHOP: return BISHOP_VALUE;
        case PieceType::ROOK: return ROOK_VALUE;
        case PieceType::QUEEN: return QUEEN_VALUE;
        case PieceType::KING: return 10'000;
        default: throw std::runtime_error("Unknown PieceType");
    }
}