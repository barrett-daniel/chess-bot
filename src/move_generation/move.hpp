#pragma once
#include "util/types.hpp"

struct Move {
    Square from;
    Square to;
    PieceType moved_piece;

    PieceType promotion_piece;
    bool is_en_passant;
    bool q_castle;
    bool k_castle;

    bool operator==(const Move&) const;

    Move();
    Move(Square from, Square to, PieceType moved_piece);
    std::string to_string() const;
};
