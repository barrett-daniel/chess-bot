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

MOVE OPTAMISATION:
- rework history into undo_structs
- precomputed move tables for bishops and rooks
- change 'Sqaure' from 'int' to 'uint8_t'
- reduce heap allocations i.e. use std::array not std::vector

AI OPTAMISATION:
- sorting moves
- hash-set for openenings
- overlay bbs for piece positions at various states for better positioning
- opening move book
- endgame move book/optimisation

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
#include "delegates/ai_manager.hpp"
#include "utility/logger.hpp"

int main(int argc, char* argv[]) {
    LOGGER_ON;
    LOG("Starting program in DEBUG mode");

    ASSERT(SDL_Init(SDL_INIT_VIDEO), "SDL failed to initalise - " << SDL_GetError());

    SDL_Window* window = SDL_CreateWindow(WINDOW_NAME, WINDOW_DIM, WINDOW_DIM, 0);
    ASSERT(window, "SDL window failed to initalise - " << SDL_GetError());

    const Colour player = Colour::WHITE;
    SDL_Renderer* SDL_renderer = SDL_CreateRenderer(window, NULL); 
    Renderer renderer(SDL_renderer, player);
    BoardManager board_manager(START_FEN);
    InputManager input_manager(player, board_manager);
    AIManager ai_manager(!player);
    initalise_attack_arrays();

    const int frame_delay = 1000 / FPS;
    uint32_t frame_start;
    int frame_time;

    while(input_manager.is_running()) {
        frame_start = SDL_GetTicks();
        Colour turn = board_manager.get_bs().turn;

        LOGGER_OFF;
        if(board_manager.is_checkmate(turn)) {
            LOGGER_ON;
            LOG(to_string(!turn) + " Won!");
            std::cout << to_string(!turn) << " Won!" << std::endl;
            input_manager.quit();
            break;
        }
        LOGGER_ON;

        if(board_manager.get_bs().turn == !player) {
            auto start = std::chrono::high_resolution_clock::now();

            Move move = ai_manager.get_move(board_manager, AI_BASE_DEPTH);

            auto end = std::chrono::high_resolution_clock::now();
            auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            std::cout << "AI calculation " << std::to_string(duration_ms) << " ms" << std::endl;;
            std::cout << "calls: " << ai_manager.calls << std::endl;

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

