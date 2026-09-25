#include "evaluation_data.hpp"

#include <algorithm>
#include <format>

#include "util/constants.hpp"

constexpr u8 MAX_PHASE   = (18 * QUEEN_PHASE) + (2 * ROOK_PHASE) * (2 * KING_PHASE);
constexpr u8 START_PHASE = 24;

[[nodiscard]] i32 EvaluationData::get_score() const {
    ASSERT(phase < MAX_PHASE, "Phase exceeds maximum possible phase value");
    u8 clamped_phase = std::min(phase, START_PHASE);
    return (score_mg * clamped_phase + score_eg * (START_PHASE - clamped_phase)) / START_PHASE;
}

std::string EvaluationData::to_string() const {
    return std::format("(score_mg = {}, score_eg = {}, phase = {})", score_mg, score_eg, phase);
}

void EvaluationData::remove_piece_score(u8 piece_type, u8 colour, Square sq) {
    const int sign = colour_sign[colour];

    score_mg -= piece_value[piece_type] * sign;
    score_eg -= piece_value[piece_type] * sign;
    score_mg -= psqt_mg[colour][piece_type][sq] * sign;
    score_eg -= psqt_eg[colour][piece_type][sq] * sign;
    phase -= phase_value[piece_type];
}

void EvaluationData::remove_piece_score(PieceType piece_type, Colour colour, Square sq) {
    remove_piece_score(std::to_underlying(piece_type), std::to_underlying(colour), sq);
}

void EvaluationData::add_piece_score(u8 piece_type, u8 colour, Square sq) {
    const int sign = colour_sign[colour];

    score_mg += piece_value[piece_type] * sign;
    score_eg += piece_value[piece_type] * sign;
    score_mg += psqt_mg[colour][piece_type][sq] * sign;
    score_eg += psqt_eg[colour][piece_type][sq] * sign;
    phase += phase_value[piece_type];
}

void EvaluationData::add_piece_score(PieceType piece_type, Colour colour, Square sq) {
    add_piece_score(std::to_underlying(piece_type), std::to_underlying(colour), sq);
}

void EvaluationData::process_move(u8 piece_type, u8 colour, Square move_from, Square move_to) {
    const int sign = colour_sign[colour];

    score_mg -= psqt_mg[colour][piece_type][move_from] * sign;
    score_mg += psqt_mg[colour][piece_type][move_to]   * sign;
    score_eg -= psqt_eg[colour][piece_type][move_from] * sign;
    score_eg += psqt_eg[colour][piece_type][move_to]   * sign;
}

void EvaluationData::process_move(PieceType piece_type, Colour colour, Square move_from, Square move_to) {
    process_move(std::to_underlying(piece_type), std::to_underlying(colour), move_from, move_to);
}
