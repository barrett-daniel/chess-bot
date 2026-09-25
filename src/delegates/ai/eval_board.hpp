#pragma once
#include "mvvlva.hpp"

#include "utility/util.hpp"


/*************************************************

                 Eval Board

*************************************************/

/**
 * Provides a score for the given position. Does not consider the game state (e.g. check, draw, checkmate)
 * only consider the pieces on the board and their relative positions
 *
 * @return score for the position
 */
static EvaluationData eval_board(const BoardState& bs) {
    int32_t score_mg = 0;
    int32_t score_eg = 0;
    uint8_t phase = 0;

    for (uint8_t c = 0; c < NUM_COLOURS; c++) {
        for (uint8_t pt = 0; pt < NUM_PIECE_TYPES; pt++) {
            const auto colour = static_cast<Colour>(c);
            const auto piece_type = static_cast<PieceType>(pt);

            Bitboard bb = bs.get_bb(colour, piece_type);

            const int parity = std::popcount(bb);
            const int sign = colour_sign[c];
            const int piece_val = piece_value[pt];

            score_mg += parity * piece_val * sign;
            score_eg += parity * piece_val * sign;
            phase += parity * phase_value[pt];

            while (bb) {
                Square sq = pop_lsb(bb);
                score_mg += sign * psqt_mg[c][pt][sq];
                score_eg += sign * psqt_eg[c][pt][sq];
            }
        }
    }

    return {score_mg, score_eg, phase};
}
