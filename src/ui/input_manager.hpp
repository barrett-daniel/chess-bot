#pragma once

#include <SDL3/SDL.h>

#include "../board_data/board_manager.hpp"

class InputManager {
    public:
        InputManager(const Colour player, BoardManager& board_manager);

        bool is_running() const;
        void quit();
        void process_inputs();

        bool is_any_piece_selected() const;
        tracked_array<Move, MAX_NUM_MOVES> get_selected_moves();

        void set_player(Colour colour);

    private:
        PieceType get_prom_choice();
        void handle_click(SDL_Event e);
        void handle_right_click();
        void handle_left_click(int x, int y);
        void handle_attempted_move(Square click_pos);
        
        Colour player;
        BoardManager& board_manager;
        bool running;
        Square selected_sq;
        SDL_Texture* highlight_tex;
};