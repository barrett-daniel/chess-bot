#include "move_generator.hpp"

#include "../utility/logger.hpp"
#include "../pieces/pieces.hpp"
#include "pieces/magic_bitboard.hpp"

tracked_array<Move, MAX_NUM_MOVES> get_moves(Colour side, BoardState& bs, PieceType type);

tracked_array<Move, MAX_NUM_MOVES> get_moves(Colour side, BoardState& bs, int pos) {
    LOG_TRACE;
    Bitboard enemy_pieces = bs.get_bb(!side, PieceType::NONE);
    Bitboard own_pieces   = bs.get_bb(side, PieceType::NONE);
    Bitboard occupancy_bb = bs.all_pieces;

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
            occupancy_bb
        );
    }

    Bitboard rook_bb = bs.get_bb(side, PieceType::ROOK);
    if(is_bit(rook_bb, pos)) {
        Bitboard from_bb = 0;
        set_bit(from_bb, pos);

        return get_rook_moves(
            from_bb,
            own_pieces,
            occupancy_bb
        );
    }

    Bitboard queen_bb = bs.get_bb(side, PieceType::QUEEN);
    if(is_bit(queen_bb, pos)) {
        Bitboard from_bb = 0;
        set_bit(from_bb, pos);

        return get_queen_moves(
            from_bb,
            own_pieces,
            occupancy_bb
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

tracked_array<Move, MAX_NUM_MOVES> get_moves(Colour side, BoardState& bs) {
    LOG_TRACE;
    LOGGER_OFF;
    tracked_array<Move, MAX_NUM_MOVES> moves;

    tracked_array<Move, MAX_NUM_MOVES> pawn_moves   = get_moves(side, bs, PieceType::PAWN);
    tracked_array<Move, MAX_NUM_MOVES> knight_moves = get_moves(side, bs, PieceType::KNIGHT);
    tracked_array<Move, MAX_NUM_MOVES> bishop_moves = get_moves(side, bs, PieceType::BISHOP);
    tracked_array<Move, MAX_NUM_MOVES> rook_moves   = get_moves(side, bs, PieceType::ROOK);
    tracked_array<Move, MAX_NUM_MOVES> queen_moves  = get_moves(side, bs, PieceType::QUEEN);
    tracked_array<Move, MAX_NUM_MOVES> king_moves   = get_moves(side, bs, PieceType::KING);

    moves.insert(moves.end(), pawn_moves.begin(), pawn_moves.end());
    moves.insert(moves.end(), knight_moves.begin(), knight_moves.end());
    moves.insert(moves.end(), bishop_moves.begin(), bishop_moves.end());
    moves.insert(moves.end(), rook_moves.begin(), rook_moves.end());
    moves.insert(moves.end(), queen_moves.begin(), queen_moves.end());
    moves.insert(moves.end(), king_moves.begin(), king_moves.end());

    LOGGER_ON;
    return moves;
}

tracked_array<Move, MAX_NUM_MOVES> get_moves(Colour side, BoardState& bs, PieceType type) {
    LOG_TRACE;
    Bitboard own_pieces_bb = bs.get_bb(side, PieceType::NONE);
    Bitboard pieces_bb     = bs.get_bb(!side, PieceType::NONE);
    Bitboard occupancy_bb  = bs.all_pieces;

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
                occupancy_bb
            );
        }
        case PieceType::ROOK: {
            Bitboard rook_bb = bs.get_bb(side, PieceType::ROOK);
            return get_rook_moves(
                rook_bb,
                own_pieces_bb,
                occupancy_bb
            );
        }
        case PieceType::QUEEN: {
            Bitboard queen_bb = bs.get_bb(side, PieceType::QUEEN);
            return get_queen_moves(
                queen_bb,
                own_pieces_bb,
                occupancy_bb
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