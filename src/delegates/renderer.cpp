#include "renderer.hpp"
#include "../utility/logger.hpp"

#include <SDL3_image/SDL_image.h>

Renderer::Renderer(SDL_Renderer* renderer, Colour player) 
  : player(player),
    renderer(renderer)
{
    LOG_TRACE;

    w_space_tex = IMG_LoadTexture(renderer, "assets/square_brown_light_png_512px.png");
    ASSERT(w_space_tex, "Failed to initialise w_space_tex: " << SDL_GetError());

    b_space_tex = IMG_LoadTexture(renderer, "assets/square_brown_dark_png_512px.png");
    ASSERT(b_space_tex, "Failed to initialise b_space_tex: " << SDL_GetError());

    w_pawn_tex = IMG_LoadTexture(renderer, "assets/w_pawn_png_512px.png");
    ASSERT(w_pawn_tex, "Failed to initialise w_pawn_tex: " << SDL_GetError());

    b_pawn_tex = IMG_LoadTexture(renderer, "assets/b_pawn_png_512px.png");
    ASSERT(b_pawn_tex, "Failed to initialise b_pawn_tex: " << SDL_GetError());

    w_knight_tex = IMG_LoadTexture(renderer, "assets/w_knight_png_512px.png");
    ASSERT(w_knight_tex, "Failed to initialise w_knight_tex: " << SDL_GetError());

    b_knight_tex = IMG_LoadTexture(renderer, "assets/b_knight_png_512px.png");
    ASSERT(b_knight_tex, "Failed to initialise b_knight_tex: " << SDL_GetError());

    w_bishop_tex = IMG_LoadTexture(renderer, "assets/w_bishop_png_512px.png");
    ASSERT(w_bishop_tex, "Failed to initialise w_bishop_tex: " << SDL_GetError());

    b_bishop_tex = IMG_LoadTexture(renderer, "assets/b_bishop_png_512px.png");
    ASSERT(b_bishop_tex, "Failed to initialise b_bishop_tex: " << SDL_GetError());

    w_rook_tex = IMG_LoadTexture(renderer, "assets/w_rook_png_512px.png");
    ASSERT(w_rook_tex, "Failed to initialise w_rook_tex: " << SDL_GetError());

    b_rook_tex = IMG_LoadTexture(renderer, "assets/b_rook_png_512px.png");
    ASSERT(b_rook_tex, "Failed to initialise b_rook_tex: " << SDL_GetError());

    w_queen_tex = IMG_LoadTexture(renderer, "assets/w_queen_png_512px.png");
    ASSERT(w_queen_tex, "Failed to initialise w_queen_tex: " << SDL_GetError());

    b_queen_tex = IMG_LoadTexture(renderer, "assets/b_queen_png_512px.png");
    ASSERT(b_queen_tex, "Failed to initialise b_queen_tex: " << SDL_GetError());

    w_king_tex = IMG_LoadTexture(renderer, "assets/w_king_png_512px.png");
    ASSERT(w_king_tex, "Failed to initialise w_king_tex: " << SDL_GetError());

    b_king_tex = IMG_LoadTexture(renderer, "assets/b_king_png_512px.png");
    ASSERT(b_king_tex, "Failed to initialise b_king_tex: " << SDL_GetError());

    highlight_tex = IMG_LoadTexture(renderer, "assets/highlight.png");
    ASSERT(highlight_tex, "Failed to initalise highlight_tex" << SDL_GetError());
}

// ----------------------------------------------
// Rendering
// ----------------------------------------------

void Renderer::render_board(const BoardState& bs) const {
    std::vector<BitboardMeta> boards = get_boards_data(bs);

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {

            {
                // render squares
                SDL_Texture* texture = (i + j) % 2 == 0 ? w_space_tex : b_space_tex;
                float x = i * CELL_SIZE;
                float y = j * CELL_SIZE;
                float length = CELL_SIZE;
                SDL_FRect destSq = {x, y, length, length}; 
                SDL_RenderTexture(renderer, texture, NULL, &destSq);
            }

            // Determine which board index to check next
            // this is inverted if the player is black to flip the POV of the rendering
            int board_i = (player == Colour::BLACK) ? (BOARD_SIZE - 1 - i) : i;
            int board_j = (player == Colour::BLACK) ? (BOARD_SIZE - 1 - j) : j;
            uint8_t bit_position = board_i + (board_j * BOARD_SIZE);

            SDL_Texture* tex = nullptr;
            for (BitboardMeta board : boards) {
                if(is_bit(board.board, bit_position)) {
                    tex = get_tex_for_piece(board.type, board.colour);
                    break;
                }
            }
            if (!tex) continue;

            {
                // Draw piece at screen coordinate (i, j)
                float offset = 0.125f;
                float x = (i * CELL_SIZE) + (offset * CELL_SIZE);
                float y = (j * CELL_SIZE) + (offset * CELL_SIZE);
                float length = CELL_SIZE - (2 * offset * CELL_SIZE);
                SDL_FRect pDst = {x, y, length, length};
                SDL_RenderTexture(renderer, tex, NULL, &pDst);
            }
        }
    }
}

SDL_Texture* Renderer::get_tex_for_piece(PieceType type, Colour side) const {
    if(side == Colour::WHITE) {
        switch(type) {
            case PAWN:   return w_pawn_tex;
            case KNIGHT: return w_knight_tex;
            case BISHOP: return w_bishop_tex;
            case ROOK:   return w_rook_tex;
            case QUEEN:  return w_queen_tex;
            case KING:   return w_king_tex;
            default:       return nullptr;
        }
    } else {
        switch(type) {
            case PAWN:   return b_pawn_tex;
            case KNIGHT: return b_knight_tex;
            case BISHOP: return b_bishop_tex;
            case ROOK:   return b_rook_tex;
            case QUEEN:  return b_queen_tex;
            case KING:   return b_king_tex;
            default:       return nullptr;
        }
    }
}

std::vector<BitboardMeta> Renderer::get_boards_data(const BoardState& bs) const {
    std::vector<BitboardMeta> data;

    // White pieces
    data.push_back({ bs.w_pawns,   PieceType::PAWN,   Colour::WHITE });
    data.push_back({ bs.w_knights, PieceType::KNIGHT, Colour::WHITE });
    data.push_back({ bs.w_bishops, PieceType::BISHOP, Colour::WHITE });
    data.push_back({ bs.w_rooks,   PieceType::ROOK,   Colour::WHITE });
    data.push_back({ bs.w_queens,  PieceType::QUEEN,  Colour::WHITE });
    data.push_back({ bs.w_king,    PieceType::KING,   Colour::WHITE });

    // Black pieces
    data.push_back({ bs.b_pawns,   PieceType::PAWN,   Colour::BLACK });
    data.push_back({ bs.b_knights, PieceType::KNIGHT, Colour::BLACK });
    data.push_back({ bs.b_bishops, PieceType::BISHOP, Colour::BLACK });
    data.push_back({ bs.b_rooks,   PieceType::ROOK,   Colour::BLACK });
    data.push_back({ bs.b_queens,  PieceType::QUEEN,  Colour::BLACK });
    data.push_back({ bs.b_king,    PieceType::KING,   Colour::BLACK });

    return data;
}


void Renderer::render_selected_moves(const std::vector<Move>& moves) const {
    for(Move move : moves) {
        uint8_t bit_position = move.to;
        int i = bit_position % BOARD_SIZE;
        int j = bit_position / BOARD_SIZE;

        int board_i = (player == Colour::BLACK) ? (BOARD_SIZE - 1 - i) : i;
        int board_j = (player == Colour::BLACK) ? (BOARD_SIZE - 1 - j) : j;

        float x = board_i * CELL_SIZE;
        float y = board_j * CELL_SIZE;
        float length = CELL_SIZE;
        SDL_FRect destSq = {x, y, length, length};
        SDL_RenderTexture(renderer, highlight_tex, NULL, &destSq);
    }
}
