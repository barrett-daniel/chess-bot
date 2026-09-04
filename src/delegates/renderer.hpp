#pragma once

#include <SDL3/SDL.h>
#include "../utility/util.hpp"


struct BitboardMeta {
    Bitboard board;
    PieceType type;
    Colour colour;
};

class Renderer {
    public:
        Renderer(SDL_Renderer* renderer, Colour player);
        void render_board(const BoardState& bs) const;
        void render_selected_moves(const std::vector<Move>& moves) const;

    private:
        SDL_Texture* get_tex_for_piece(PieceType type, Colour side) const;
        std::vector<BitboardMeta> get_boards_data(const BoardState& bs) const;

        Colour player;
        SDL_Renderer* renderer;
        SDL_Texture* w_space_tex;
        SDL_Texture* b_space_tex;
        SDL_Texture* w_pawn_tex;
        SDL_Texture* b_pawn_tex;
        SDL_Texture* w_knight_tex;
        SDL_Texture* b_knight_tex;
        SDL_Texture* w_bishop_tex;
        SDL_Texture* b_bishop_tex;
        SDL_Texture* w_rook_tex;
        SDL_Texture* b_rook_tex;
        SDL_Texture* w_queen_tex;
        SDL_Texture* b_queen_tex;
        SDL_Texture* w_king_tex;
        SDL_Texture* b_king_tex;
        SDL_Texture* highlight_tex;
};