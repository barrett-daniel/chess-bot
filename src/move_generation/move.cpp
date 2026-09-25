//
// Created by dan12 on 25/09/2026.
//


#include "move.hpp"

#include "../util/constants.hpp"
#include "../util/types.hpp"

bool Move::operator==(const Move&) const = default;

Move::Move() :
    from(NULL_SQ),
    to(NULL_SQ),
    moved_piece(PieceType::NONE),
    promotion_piece(PieceType::NONE),
    is_en_passant(false),
    q_castle(false),
    k_castle(false)
{}

Move::Move(Square from, Square to, PieceType moved_piece) :
    from(from),
    to(to),
    moved_piece(moved_piece),
    promotion_piece(PieceType::NONE),
    is_en_passant(false),
    q_castle(false),
    k_castle(false)
{}

std::string Move::to_string() const {
    std::string s = to_string_sq(from) + to_string_sq(to);
    if (promotion_piece != PieceType::NONE) {
        switch (promotion_piece) {
            case PieceType::QUEEN:  s += 'q'; break;
            case PieceType::ROOK:   s += 'r'; break;
            case PieceType::BISHOP: s += 'b'; break;
            case PieceType::KNIGHT: s += 'n'; break;
            default: break;
        }
    }
    return s;
}