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

        int eval_board(const BoardState &bs);

};
