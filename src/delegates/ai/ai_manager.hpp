#pragma once

#include <chrono>

#include "../../utility/util.hpp"
#include "../board_manager.hpp"
#include "../move_generator.hpp"
#include "transposition_table.hpp"

struct SearchData {
    int depth;
    uint64_t nodes;
    std::chrono::microseconds elapsed_time;
};

class AIManager {
    public:
        AIManager(Colour ai_colour, BoardManager& board_manager, int search_limit_ms);

        Move get_move(SearchData* search_data = nullptr);

    private:
        BoardManager& board_manager;
        Colour ai_colour;

        // search optimisation members
        TranspositionTable tt;
        int history_heuristic[NUM_SQUARES][NUM_SQUARES];
        Move killers[AI_MAX_DEPTH][2];

        // timeout control members
        std::chrono::steady_clock::time_point search_start;
        std::chrono::milliseconds search_limit;
        bool search_aborted;

        // private functions
        int negamax(int depth, int alpha, int beta, uint64_t& num_visited_nodes);
        int eval_board(const BoardState &bs) const;
        void order_moves(tracked_array<Move, MAX_NUM_MOVES>& moves, const Move& tt_best_move, int depth, const BoardState& bs) const;

};
