#include "input_manager.hpp"
#include "../utility/logger.hpp"
#include "../pieces/pieces.hpp"

#include <SDL3_image/SDL_image.h>

InputManager::InputManager(const Colour player, BoardManager& board_manager) 
  : player(player),
    board_manager(board_manager),
    running(true),
    selected_sq(NULL_SQ)
{
    LOG_TRACE;
}

bool InputManager::is_running() const {
    return running;
}

void InputManager::quit() {
    running = false;
}

void InputManager::process_inputs() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        switch(e.type) {
            case SDL_EVENT_QUIT: running = false; break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN: handle_click(e); break;
            case SDL_EVENT_KEY_DOWN: if(e.key.key == SDLK_LEFT) board_manager.undo_move(); board_manager.undo_move();
        }
    }
}

void InputManager::handle_click(SDL_Event e) {
    ASSERT(e.type == SDL_EVENT_MOUSE_BUTTON_DOWN, "Improper event management function call");

    switch(e.button.button) {
        case SDL_BUTTON_RIGHT: handle_right_click(); break;
        case SDL_BUTTON_LEFT: 
            int x =  e.button.x / CELL_SIZE;
            int y =  e.button.y / CELL_SIZE;
            handle_left_click(x, y);
            break;
    }
}

void InputManager::handle_right_click() {
    LOG_TRACE;
    selected_sq = NULL_SQ;
}

void InputManager::handle_left_click(int x, int y) {
    LOG_TRACE;

    // translate click_pos for black
    int board_x = (player == Colour::BLACK) ? (BOARD_SIZE - 1 - x) : x;
    int board_y = (player == Colour::BLACK) ? (BOARD_SIZE - 1 - y) : y;
    Square click_pos = board_x + (board_y * BOARD_SIZE);
    LOG("Click identified at: " + to_string_coord(click_pos));

    Bitboard own_pieces = board_manager.get_bs().get_bb(player, PieceType::NONE);
    if(is_bit(own_pieces, click_pos)) {
        selected_sq = click_pos;
        LOG("Square selected: " + to_string_coord(selected_sq));
        return;
    }

    // given a square is already selected, assume the player is trying to
    // make a move, and attempt to make it
    if(selected_sq != NULL_SQ) handle_attempted_move(click_pos);
}

tracked_array<Move, MAX_NUM_MOVES> InputManager::get_selected_moves() {
    auto moves = get_moves(player, board_manager.get_bs(), selected_sq);

    for(Move move : moves) {
        LOG("Move identified for selected piece - from: " + to_string_coord(move.from) + " to: " + to_string_coord(move.to));
    }
    return board_manager.get_legal_moves(moves);
}

void InputManager::handle_attempted_move(Square click_pos) {
    LOG_TRACE;

    tracked_array<Move, MAX_NUM_MOVES> legal_moves = get_selected_moves();
    for(Move move : legal_moves) {
        if(move.to == click_pos) {
            if(move.promotion_piece != NONE) {
                PieceType prom_type = get_prom_choice();
                move.promotion_piece = prom_type;
            }

            LOG("The player attempted to make a valid move -> make the move");
            board_manager.make_move(move);
            return;
        }
    }
    LOG("The player's attempted move was not valid");
}

bool InputManager::is_any_piece_selected() const {
    return selected_sq != NULL_SQ;
}

PieceType InputManager::get_prom_choice() {
    while(true) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_EVENT_KEY_DOWN) {
                switch(e.key.key) {
                    case SDLK_Q: return PieceType::QUEEN;
                    case SDLK_B: return PieceType::BISHOP;
                    case SDLK_R: return PieceType::ROOK;
                    case SDLK_K: return PieceType::KNIGHT;
                    default: break;
                }
            }
        }
    }
    return PieceType::NONE;
}