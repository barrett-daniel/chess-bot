#pragma once

#include "../utility/util.hpp"
#include "board_manager.hpp"
#include "move_generator.hpp"

class AIManager {
    public:
        AIManager(Colour ai_colour);
        Move get_move(BoardManager& board_manager, int depth);
        int min_max(BoardManager& board_manager, int depth, bool is_maximising, int alpha, int beta);
        int min_max(BoardManager& board_manager, int depth, bool is_maximising);
        
        int calls;

    private:
        Colour ai_colour;

        int eval_board(BoardState& bs);

};