/*
 * Assets by JohnPablok - https://opengameart.org/content/chess-pieces-and-board-squares
 * Obtained under CC BY-SA 3.0
 * 
 * https://wiki.libsdl.org/SDL3/APIByCategory
 */

/*
To Do:

OTHER:
- GUI improvements:
    - Options of how to use app: play against another person, play against bot, etc.
    - offload AI work to background thread to not buffer main thread
    - cache is in check / stalemate?
    - 3-fold repetition
- could optimise valid_castle by removing delta_bb by using precomputed constant BBs for king path
- only two kings on board == draw

MOVE OPTAMISATION:
-

AI OPTAMISATION:
- Opening Book moves
- Endgame move book/optimisation
- Quiescence search
- Null-move pruning
- Incremental evaluation
- Mobility

1. Incremental board evaluation
2. Selective Search Pruning
3. Optimise bottlenecks (believe calculating pinned squares is one
4. piece_at array in board_state

https://chessprogramming.org/Encoding_Moves#fn2
https://chessprogramming.org/Perft
https://chessprogramming.org/Perft_Results#position-3
https://chessprogramming.org/UCI
https://grandchesstree.com/tools

*/

#include <SDL3/SDL.h>
#include <chrono>

#include "pieces/pawn.hpp"
#include "delegates/board_manager.hpp"
#include "delegates/renderer.hpp"
#include "delegates/input_manager.hpp"
#include "delegates/move_generator.hpp"
#include "delegates/ai/ai_manager.hpp"
#include "utility/zobrist_hashing.hpp"
#include "utility/logger.hpp"
#include "utility/start_up.hpp"

int main(int argc, char* argv[]) {
    LOGGER_ON;
    LOG("Starting program in DEBUG mode");

    ASSERT(SDL_Init(SDL_INIT_VIDEO), "SDL failed to initalise - " << SDL_GetError());

    SDL_Window* window = SDL_CreateWindow(WINDOW_NAME, WINDOW_DIM, WINDOW_DIM, 0);
    ASSERT(window, "SDL window failed to initalise - " << SDL_GetError());

    init_static_data();

    const Colour player = Colour::WHITE;
    SDL_Renderer* SDL_renderer = SDL_CreateRenderer(window, NULL); 
    Renderer renderer(SDL_renderer, player);
    BoardManager board_manager(START_FEN);
    InputManager input_manager(player, board_manager);
    AIManager ai_manager(!player, board_manager, AI_SEARCH_TIME_MS);

    const int frame_delay = 1000 / FPS;
    uint32_t frame_start;
    int frame_time;

    while(input_manager.is_running()) {
        frame_start = SDL_GetTicks();
        Colour turn = board_manager.get_bs().turn;

        LOGGER_OFF;
        GameState state = board_manager.get_game_state();
        if (is_game_over(state)) {
            LOGGER_ON;
            LOG("Game Over! State: " + std::to_string(static_cast<uint8_t>(state)));
            input_manager.quit();
            break;
        }
        LOGGER_ON;

        if(board_manager.get_bs().turn == !player) {
            Move move = ai_manager.get_move();
            board_manager.make_move(move);
        }

        input_manager.process_inputs();

        LOGGER_OFF;
        SDL_RenderClear(SDL_renderer);

        renderer.render_board(board_manager.get_bs());
        if(input_manager.is_any_piece_selected()) {
            renderer.render_selected_moves(input_manager.get_selected_moves());
        }
        
        SDL_RenderPresent(SDL_renderer);
        LOGGER_ON;

        frame_time = SDL_GetTicks() - frame_start;
        if (frame_delay > frame_time) SDL_Delay(frame_delay - frame_time);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

