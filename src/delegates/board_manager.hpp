#pragma once

#include <stack>

#include "../utility/util.hpp"
#include "move_generator.hpp"

class BoardManager {
    public:
        BoardManager() = default;
        BoardManager(const std::string& fen);

        void make_move(const Move& move);
        void undo_move();
        tracked_array<Move, MAX_NUM_MOVES> get_legal_moves(const tracked_array<Move, MAX_NUM_MOVES>& moves, bool in_check);
        tracked_array<Move, MAX_NUM_MOVES> get_legal_moves(const tracked_array<Move, MAX_NUM_MOVES>& moves);

        GameState get_game_state();
        bool is_fifty_move_rule() const;
        bool is_three_fold_repetition() const;
        bool is_insufficient_material() const;
        bool is_in_check(Colour side) const;

        BoardState& get_bs();

    private:
        void inline update_castling_rights(Square move_pos);
        void inline update_poly_bbs();

        bool is_pinned(Square from);
        tracked_array<Square, BOARD_SIZE> calc_pinned_sqs();
        inline bool same_line(Square from, Square to, int dir);

        void process_q_castle(Colour side);
        void process_k_castle(Colour side);
        void process_castle(
            Square rook_from, Square rook_to,
            Square king_from, Square king_to, Colour side
        );

        bool valid_q_castle(Square move_from, Colour side, bool in_check);
        bool valid_k_castle(Square move_from, Colour side, bool in_check);
        bool valid_castle(Square move_from, Square rook_pos, Colour side, bool king_castle);
        Bitboard get_delta_bb(Square a, Square b);

        BoardState bs;
        std::vector<BoardState> history;
};