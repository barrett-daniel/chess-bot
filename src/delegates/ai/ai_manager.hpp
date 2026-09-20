#pragma once

#include <chrono>

#include "../../utility/util.hpp"
#include "../board_manager.hpp"
#include "../move_generator.hpp"
#include "transposition_table.hpp"

class AIManager {
    public:
        AIManager(Colour ai_colour, BoardManager& board_manager);

        std::pair<Move, int> get_move_until_time(int max_duration_ms);
        int negamax(int depth, int alpha, int beta);

    private:
        BoardManager& board_manager;
        Colour ai_colour;
        TranspositionTable tt;
        int history_heuristic[NUM_SQUARES][NUM_SQUARES];
        Move killers[AI_MAX_DEPTH][2];

        int eval_board(const BoardState &bs) const;
        void order_moves(tracked_array<Move, MAX_NUM_MOVES>& moves, const Move& tt_best_move, int depth, const BoardState& bs) const;

};
