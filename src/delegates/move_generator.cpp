#include "move_generator.hpp"

#include "../utility/logger.hpp"
#include "../pieces/pieces.hpp"

std::vector<Move> get_moves(Colour side, BoardState& bs, int pos) {
    LOG_TRACE;
    Bitboard enemy_pieces = bs.get_bb(!side, PieceType::NONE);
    Bitboard own_pieces   = bs.get_bb(side, PieceType::NONE);

    Bitboard pawn_bb = bs.get_bb(side, PieceType::PAWN);
    if(is_bit(pawn_bb, pos)) {
        Bitboard from_bb = 0;
        set_bit(from_bb, pos);

        return get_pawn_moves(
            from_bb,
            side == Colour::WHITE,
            enemy_pieces,
            bs.all_pieces,
            bs.en_passant_square
        );
    }

    Bitboard knight_bb = bs.get_bb(side, PieceType::KNIGHT);
    if(is_bit(knight_bb, pos)) {
        Bitboard from_bb = 0;
        set_bit(from_bb, pos);

        return get_knight_moves(
            from_bb,
            own_pieces
        );
    }

    Bitboard bishop_bb = bs.get_bb(side, PieceType::BISHOP);
    if(is_bit(bishop_bb, pos)) {
        Bitboard from_bb = 0;
        set_bit(from_bb, pos);

        return get_bishop_moves(
            from_bb,
            own_pieces,
            enemy_pieces
        );
    }

    Bitboard rook_bb = bs.get_bb(side, PieceType::ROOK);
    if(is_bit(rook_bb, pos)) {
        Bitboard from_bb = 0;
        set_bit(from_bb, pos);

        return get_rook_moves(
            from_bb,
            own_pieces,
            enemy_pieces
        );
    }

    Bitboard queen_bb = bs.get_bb(side, PieceType::QUEEN);
    if(is_bit(queen_bb, pos)) {
        Bitboard from_bb = 0;
        set_bit(from_bb, pos);

        return get_queen_moves(
            from_bb,
            own_pieces,
            enemy_pieces
        );
    }

    Bitboard king_bb = bs.get_bb(side, PieceType::KING);
    if(is_bit(king_bb, pos)) {
        Bitboard from_bb = 0;
        set_bit(from_bb, pos);

        return get_king_moves(
            from_bb,
            own_pieces,
            bs.castling_rights,
            side == Colour::WHITE
        );
    }
    return {};
}

std::vector<Move> get_moves(Colour side, BoardState& bs) {
    LOG_TRACE;
    LOGGER_OFF;
    std::vector<Move> moves;

    std::vector<Move> pawn_moves   = get_moves(side, bs, PieceType::PAWN);
    std::vector<Move> knight_moves = get_moves(side, bs, PieceType::KNIGHT);
    std::vector<Move> bishop_moves = get_moves(side, bs, PieceType::BISHOP);
    std::vector<Move> rook_moves   = get_moves(side, bs, PieceType::ROOK);
    std::vector<Move> queen_moves  = get_moves(side, bs, PieceType::QUEEN);
    std::vector<Move> king_moves   = get_moves(side, bs, PieceType::KING);

    LOGGER_ON;
    return combine(pawn_moves, knight_moves, bishop_moves, rook_moves, queen_moves, king_moves);
}

std::vector<Move> get_moves(Colour side, BoardState& bs, PieceType type) {
    LOG_TRACE;
    Bitboard own_pieces_bb = bs.get_bb(side, PieceType::NONE);
    Bitboard pieces_bb     = bs.get_bb(!side, PieceType::NONE);

    switch(type) {
        case PieceType::PAWN: {
            Bitboard pawn_bb = bs.get_bb(side, PieceType::PAWN);
            return get_pawn_moves(
                pawn_bb,
                side == Colour::WHITE,
                pieces_bb,
                bs.all_pieces,
                bs.en_passant_square
            );
        }
        case PieceType::KNIGHT: {
            Bitboard knight_bb = bs.get_bb(side, PieceType::KNIGHT);
            return get_knight_moves(
                knight_bb,
                own_pieces_bb
            );
        }
        case PieceType::BISHOP: {
            Bitboard bishop_bb = bs.get_bb(side, PieceType::BISHOP);
            return get_bishop_moves(
                bishop_bb,
                own_pieces_bb,
                pieces_bb
            );
        }
        case PieceType::ROOK: {
            Bitboard rook_bb = bs.get_bb(side, PieceType::ROOK);
            return get_rook_moves(
                rook_bb,
                own_pieces_bb,
                pieces_bb
            );
        }
        case PieceType::QUEEN: {
            Bitboard queen_bb = bs.get_bb(side, PieceType::QUEEN);
            return get_queen_moves(
                queen_bb,
                own_pieces_bb,
                pieces_bb
            );
        }
        case PieceType::KING: {
            Bitboard king_bb = bs.get_bb(side, PieceType::KING);
            return get_king_moves(
                king_bb,
                own_pieces_bb,
                bs.castling_rights,
                side == Colour::WHITE
            );
        }
        default: break;
    }
    throw std::invalid_argument("get_moves called with unspecified piece");
}

void initalise_attack_arrays() {
    init_knight_attacks();
    init_king_attacks();
    init_pawn_attacks(false);
    init_pawn_attacks(true);
}