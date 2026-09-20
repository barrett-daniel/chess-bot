#pragma once

#include "../utility/util.hpp"
#include "board_manager.hpp"
#include "move_generator.hpp"
#include "transposition_table.hpp"

class AIManager {
    public:
        AIManager(Colour ai_colour);
        Move get_move(BoardManager& board_manager, int depth);
        int negamax(BoardManager& board_manager, int depth, int alpha, int beta);

    private:
        Colour ai_colour;
        TranspositionTable tt;
        int history_heuristic[NUM_SQUARES][NUM_SQUARES];
        Move killers[AI_BASE_DEPTH][2];

        int eval_board(const BoardState &bs) const;
        void order_moves(tracked_array<Move, MAX_NUM_MOVES>& moves, const Move& tt_best_move, int depth, const BoardState& bs);
        bool is_capture(const Move& move, const BoardState& bs);
        int mvv_lva_score(const Move& move, const BoardState& bs);

};
