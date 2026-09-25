#pragma once

#include "util/types.hpp"

struct EvaluationData {
    i32 score_mg;
    i32 score_eg;
    u8 phase;

    /**
     *
     * @return the interpolated score between middle and endgame scores based on the phase (heuristic for amount of material on board)
     */
    [[nodiscard]] i32 get_score() const;

    void remove_piece_score(u8 piece_type, u8 colour, Square sq);
    void remove_piece_score(PieceType piece_type, Colour colour, Square sq);

    void add_piece_score(u8 piece_type, u8 colour, Square sq);
    void add_piece_score(PieceType piece_type, Colour colour, Square sq);

    void process_move(u8 piece_type, u8 colour, Square move_from, Square move_to);
    void process_move(PieceType piece_type, Colour colour, Square move_from, Square move_to);

    bool operator ==(const EvaluationData&) const = default;
    std::string to_string() const;
};

inline std::ostream& operator<<(std::ostream& os, const EvaluationData& ed) {
    os << "EvaluationData{"
       << "score_mg=" << ed.score_mg
       << ", score_eg=" << ed.score_eg
       << ", phase=" << static_cast<int>(ed.phase)  // u8 would print as a char otherwise
       << "}";
    return os;
}