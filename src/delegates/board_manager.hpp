#pragma once

#include <stack>

#include "../utility/util.hpp"
#include "move_generator.hpp"

class BoardManager {
    public:
        BoardManager(std::string fen);

        void make_move(const Move& move);
        void undo_move();
        std::vector<Move> get_legal_moves(const std::vector<Move>& moves);

        GameState get_game_state();
        bool is_in_check(Colour side);

        BoardState& get_bs();

    private:
        void inline update_castling_rights(int move_pos);
        void inline update_poly_bbs();

        bool is_pinned(int from);
        std::vector<int> calc_pinned_sqs();
        inline bool same_line(int from, int to, int dir);

        void process_q_castle(Colour side);
        void process_k_castle(Colour side);
        void process_castle(
            int rook_from, int rook_to, 
            int king_from, int king_to, Colour side
        );

        bool valid_q_castle(int move_from, Colour side, bool in_check);
        bool valid_k_castle(int move_from, Colour side, bool in_check);
        bool valid_castle(int move_from, int rook_pos, Colour side, bool king_castle);
        Bitboard get_delta_bb(int a, int b);

        BoardState bs;
        std::stack<BoardState> history; 
};