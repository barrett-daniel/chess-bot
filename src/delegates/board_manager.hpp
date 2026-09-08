#pragma once

#include <stack>

#include "../utility/util.hpp"
#include "move_generator.hpp"

class BoardManager {
    public:
        BoardManager() = default;
        BoardManager(std::string fen);

        void make_move(const Move& move);
        void undo_move();
        std::vector<Move> get_legal_moves(const std::vector<Move>& moves, bool in_check);
        std::vector<Move> get_legal_moves(const std::vector<Move>& moves);

        static GameState get_game_state(const std::vector<Move>& legal_moves, bool in_check);
        GameState get_game_state();
        bool is_in_check(Colour side);

        BoardState& get_bs();

    private:
        void inline update_castling_rights(Square move_pos);
        void inline update_poly_bbs();

        bool is_pinned(Square from);
        std::vector<Square> calc_pinned_sqs();
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
        std::stack<BoardState> history; 
};