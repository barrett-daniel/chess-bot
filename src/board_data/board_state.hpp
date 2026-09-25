#pragma once

#include <array>

#include "util/constants.hpp"
#include "evaluation_data.hpp"
#include "../move_generation/move.hpp"
#include "util/types.hpp"

constexpr int NUM_PIECE_BBS = 12;
struct BoardState {
        // Piece bitboards are stored as an array of the form:
        // [white pawn, white knight, ... all white pieces,
        //  black pawn, black knight, ... all black pieces]
        // Based on the casted index of the PieceType and Colour enums
        // get_bb allows a clean interface to index into array and fetch
        // the associated bitboard based on the desired PieceType and Colour
        std::array<Bitboard, 14> bbs;
        Bitboard all_pieces;

        Colour turn;
        u8 castling_rights;
        Square en_passant_square;
        tracked_array<Square, BOARD_SIZE> pinned_sqs; // max number of pinned pieces is 8 as there are only 8 directions

        u16 full_move_count;
        u8 half_move_count; // half move logic for 50-move draw rule is not yet implemented

        u64 hash;
        EvaluationData edata;

        BoardState();

        Bitboard& get_bb(Colour side, PieceType type);
        Bitboard get_bb(Colour side, PieceType type) const;

        bool is_capture(const Move& move) const;

        std::string to_string() const;
        std::string to_fen() const;
};