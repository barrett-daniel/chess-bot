#pragma once

#include <array>
#include <iostream>
#include <vector>

#include "types.hpp"

/* ***********************************************

                        FEN

*********************************************** */

constexpr const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr const char* BLANK_FEN = "8/8/8/8/8/8/8/8";
constexpr int FEN_NUM_SECTIONS = 2;
constexpr int FEN_BOARD_CONFIG= 0;
constexpr int FEN_TURN_CONFIG = 1;
constexpr int FEN_CASTLING_CONFIG = 2;
constexpr int FEN_EN_PASSANT_CONFIG = 3;
constexpr int FEN_HALFMOVE_CONFIG = 4;
constexpr int FEN_FULLMOVE_CONFIG = 5;

/* ***********************************************

                        AI

*********************************************** */

constexpr int AI_MAX_DEPTH = 50;
constexpr int AI_SEARCH_TIME_MS = 2000;

/* ***********************************************

                    ASSERT

*********************************************** */

// an assert wrapper which compiles to a no-op in release mode inducing no
// runtime overhead

#ifdef NDEBUG
    #define ASSERT(expr, msg) ((void)0)
#else
    #define \
    ASSERT(expr, msg) \
        if (!(expr)) { \
        std::cerr << "*****" << std::endl << \
        "Assertion failed" << std::endl << \
        "File: " << __FILE__ << " - " << \
        "Line: " << __LINE__ << std::endl << \
        "Expression: \"" << #expr << "\" - Message: " << msg << std::endl \
        << "*****" << std::endl; \
        std::abort(); \
    }
#endif

/* ***********************************************

                Bitboards Constants

*********************************************** */

constexpr Bitboard RANK_8(0xff00000000000000);
constexpr Bitboard RANK_7(0xff000000000000);
constexpr Bitboard RANK_6(0xff0000000000);
constexpr Bitboard RANK_5(0xff00000000);
constexpr Bitboard RANK_4(0xff000000);
constexpr Bitboard RANK_3(0xff0000);
constexpr Bitboard RANK_2(0xff00);
constexpr Bitboard RANK_1(0xff);

constexpr Bitboard FILE_A(0x101010101010101);
constexpr Bitboard FILE_B(0x202020202020202);
constexpr Bitboard FILE_C(0x404040404040404);
constexpr Bitboard FILE_D(0x808080808080808);
constexpr Bitboard FILE_E(0x1010101010101010);
constexpr Bitboard FILE_F(0x2020202020202020);
constexpr Bitboard FILE_G(0x4040404040404040);
constexpr Bitboard FILE_H(0x8080808080808080);

constexpr Bitboard EMPTY(0x0);
constexpr Bitboard FULL(0xffffffffffffffff);

/* ***********************************************

                    Squares

*********************************************** */

constexpr Square NULL_SQ = 64;
constexpr u8 NUM_SQUARES = 64;
constexpr int BOARD_SIZE = 8;

// The maxmimum number of moves from any reachable chess position from the
// start position. This value is know.
// source: https://lichess.org/@/Tobs40/blog/why-a-reachable-position-can-have-at-most-218-playable-moves/a5xdxeqs;
constexpr u8 MAX_NUM_MOVES = 218;

// Below are aliases for each square on the board. They're primarily used for
// testing to reconstruct certain positions after a series of moves.

constexpr Square A1 = 0;
constexpr Square B1 = 1;
constexpr Square C1 = 2;
constexpr Square D1 = 3;
constexpr Square E1 = 4;
constexpr Square F1 = 5;
constexpr Square G1 = 6;
constexpr Square H1 = 7;

constexpr Square A2 = 8;
constexpr Square B2 = 9;
constexpr Square C2 = 10;
constexpr Square D2 = 11;
constexpr Square E2 = 12;
constexpr Square F2 = 13;
constexpr Square G2 = 14;
constexpr Square H2 = 15;

constexpr Square A3 = 16;
constexpr Square B3 = 17;
constexpr Square C3 = 18;
constexpr Square D3 = 19;
constexpr Square E3 = 20;
constexpr Square F3 = 21;
constexpr Square G3 = 22;
constexpr Square H3 = 23;

constexpr Square A4 = 24;
constexpr Square B4 = 25;
constexpr Square C4 = 26;
constexpr Square D4 = 27;
constexpr Square E4 = 28;
constexpr Square F4 = 29;
constexpr Square G4 = 30;
constexpr Square H4 = 31;

constexpr Square A5 = 32;
constexpr Square B5 = 33;
constexpr Square C5 = 34;
constexpr Square D5 = 35;
constexpr Square E5 = 36;
constexpr Square F5 = 37;
constexpr Square G5 = 38;
constexpr Square H5 = 39;

constexpr Square A6 = 40;
constexpr Square B6 = 41;
constexpr Square C6 = 42;
constexpr Square D6 = 43;
constexpr Square E6 = 44;
constexpr Square F6 = 45;
constexpr Square G6 = 46;
constexpr Square H6 = 47;

constexpr Square A7 = 48;
constexpr Square B7 = 49;
constexpr Square C7 = 50;
constexpr Square D7 = 51;
constexpr Square E7 = 52;
constexpr Square F7 = 53;
constexpr Square G7 = 54;
constexpr Square H7 = 55;

constexpr Square A8 = 56;
constexpr Square B8 = 57;
constexpr Square C8 = 58;
constexpr Square D8 = 59;
constexpr Square E8 = 60;
constexpr Square F8 = 61;
constexpr Square G8 = 62;
constexpr Square H8 = 63;

/* ***********************************************

                    Castling

*********************************************** */

// castling rights masks

constexpr u8 W_QUEEN_CASTLE_MASK = 0b00001000;
constexpr u8 W_KING_CASTLE_MASK  = 0b00000100;
constexpr u8 B_QUEEN_CASTLE_MASK = 0b00000010;
constexpr u8 B_KING_CASTLE_MASK  = 0b00000001;

// castling square aliases

constexpr Square W_KING_START_POS = E1;
constexpr Square B_KING_START_POS = E8;

constexpr Square W_QUEEN_ROOK_START_POS = A1;
constexpr Square W_KING_ROOK_START_POS  = H1;
constexpr Square B_QUEEN_ROOK_START_POS = A8;
constexpr Square B_KING_ROOK_START_POS  = H8;

constexpr Square W_QUEEN_ROOK_CASTLE_TO  = D1;
constexpr Square W_KING_ROOK_CASTLE_TO   = F1;
constexpr Square B_QUEEN_ROOK_CASTLE_TO  = D8;
constexpr Square B_KING_ROOK_CASTLE_TO   = F8;

constexpr Square W_KING_CASTLE_TO  = G1;
constexpr Square W_QUEEN_CASTLE_TO = C1;
constexpr Square B_KING_CASTLE_TO  = G8;
constexpr Square B_QUEEN_CASTLE_TO = C8;


/* ***********************************************

            Attack Tables and Move Directions

*********************************************** */

// precomputed bitboards of: for the given index (which is a square on the board), 1 if a piece of the
// given type is able to attack the square index, 0 if not.

inline Bitboard KNIGHT_ATTACKS[BOARD_SIZE * BOARD_SIZE];
inline Bitboard PAWN_ATTACKS_W[BOARD_SIZE * BOARD_SIZE];
inline Bitboard PAWN_ATTACKS_B[BOARD_SIZE * BOARD_SIZE];
inline Bitboard KING_ATTACKS[BOARD_SIZE * BOARD_SIZE];

inline constexpr std::array<std::pair<int, Bitboard>, 4> ROOK_DIRECTIONS = {{
    { 8, EMPTY},
    {-8, EMPTY},
    { 1, FILE_H},
    {-1, FILE_A},
}};

inline constexpr std::array<std::pair<int, Bitboard>, 4> BISHOP_DIRECTIONS = {{
    { 9, FILE_H},
    {-7, FILE_H},
    {-9, FILE_A},
    { 7, FILE_A},
}};

inline constexpr std::array<std::pair<int, Bitboard>, 8> KING_DIRECTIONS = {{
    {8,   EMPTY},
    {9,   FILE_H},
    {1,   FILE_H},
    {-7,  FILE_H},
    {-8,  EMPTY},
    {-9,  FILE_A},
    {-1,  FILE_A},
    {7,   FILE_A},
}};

inline constexpr std::array<std::pair<int, Bitboard>, 8> KNIGHT_DIRECTIONS = {{
    {17,  FILE_H},
    {10,  FILE_G | FILE_H},
    {-6,  FILE_G | FILE_H},
    {-15, FILE_H},
    {-17, FILE_A},
    {-10, FILE_A | FILE_B},
    {6,   FILE_A | FILE_B},
    {15,  FILE_A},
}};

/* ***********************************************

                    Rendering

*********************************************** */

constexpr const char* WINDOW_NAME = "Chess";
constexpr int CELL_SIZE = 128;
constexpr int WINDOW_DIM = BOARD_SIZE * CELL_SIZE;
constexpr int FPS = 30;

/* ***********************************************

            Evaluation Data Constants

*********************************************** */

constexpr int PAWN_VALUE   = 100;
constexpr int KNIGHT_VALUE = 300;
constexpr int BISHOP_VALUE = 310;
constexpr int ROOK_VALUE   = 500;
constexpr int QUEEN_VALUE  = 900;
constexpr int KING_VALUE   = 0;

constexpr i32 piece_value[NUM_PIECE_TYPES] = {
    PAWN_VALUE,
    KNIGHT_VALUE,
    BISHOP_VALUE,
    ROOK_VALUE,
    QUEEN_VALUE,
    KING_VALUE
};

constexpr int PAWN_PHASE   = 0;
constexpr int KNIGHT_PHASE = 1;
constexpr int BISHOP_PHASE = 1;
constexpr int ROOK_PHASE   = 2;
constexpr int QUEEN_PHASE  = 4;
constexpr int KING_PHASE   = 0;

constexpr u8 phase_value[NUM_PIECE_TYPES] = {
    PAWN_PHASE,
    KNIGHT_PHASE,
    BISHOP_PHASE,
    ROOK_PHASE,
    QUEEN_PHASE,
    KING_PHASE
};

constexpr int psqt_mg[NUM_COLOURS][NUM_PIECE_TYPES][NUM_SQUARES] = {
    {
        {
            0,  0,  0,  0,  0,  0,  0,  0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5,  5, 10, 25, 25, 10,  5,  5,
            0,  0,  0, 20, 20,  0,  0,  0,
            5, -5,-10,  0,  0,-10, -5,  5,
            5, 10, 10,-20,-20, 10, 10,  5,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {
            -50,-40,-30,-30,-30,-30,-40,-50,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -30,  0, 10, 15, 15, 10,  0,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  0, 15, 20, 20, 15,  0,-30,
            -30,  5, 10, 15, 15, 10,  5,-30,
            -40,-20,  0,  5,  5,  0,-20,-40,
            -50,-40,-30,-30,-30,-30,-40,-50
        },
        {
            -20,-10,-10,-10,-10,-10,-10,-20,
            -10,  5,  0,  0,  0,  0,  5,-10,
            -10, 10, 10, 10, 10, 10, 10,-10,
            -10,  0, 10, 10, 10, 10,  0,-10,
            -10,  5,  5, 10, 10,  5,  5,-10,
            -10,  0,  5, 10, 10,  5,  0,-10,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -20,-10,-10,-10,-10,-10,-10,-20
        },
        {
            0,  0,  0,  0,  0,  0,  0,  0,
            5, 10, 10, 10, 10, 10, 10,  5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            0,  0,  0,  5,  5,  0,  0,  0
        },
        {
            -20,-10,-10, -5, -5,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5,  5,  5,  5,  0,-10,
            -5,  0,  5,  5,  5,  5,  0, -5,
            0,  0,  5,  5,  5,  5,  0, -5,
            -10,  5,  5,  5,  5,  5,  0,-10,
            -10,  0,  5,  0,  0,  0,  0,-10,
            -20,-10,-10, -5, -5,-10,-10,-20
        },
        {
            20, 30, 10,  0,  0, 10, 30, 20,
            20, 20,  0,  0,  0,  0, 20, 20,
            -10,-20,-20,-20,-20,-20,-20,-10,
            -20,-30,-30,-40,-40,-30,-30,-20,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30
        }
    },
    {
        {
            0,  0,  0,  0,  0,  0,  0,  0,
            5, 10, 10,-20,-20, 10, 10,  5,
            5, -5,-10,  0,  0,-10, -5,  5,
            0,  0,  0, 20, 20,  0,  0,  0,
            5,  5, 10, 25, 25, 10,  5,  5,
            10, 10, 20, 30, 30, 20, 10, 10,
            50, 50, 50, 50, 50, 50, 50, 50,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {
            -50,-40,-30,-30,-30,-30,-40,-50,
            -40,-20,  0,  5,  5,  0,-20,-40,
            -30,  5, 10, 15, 15, 10,  5,-30,
            -30,  0, 15, 20, 20, 15,  0,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  0, 10, 15, 15, 10,  0,-30,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -50,-40,-30,-30,-30,-30,-40,-50
        },
        {
            -20,-10,-10,-10,-10,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5, 10, 10,  5,  0,-10,
            -10,  5,  5, 10, 10,  5,  5,-10,
            -10,  0, 10, 10, 10, 10,  0,-10,
            -10, 10, 10, 10, 10, 10, 10,-10,
            -10,  5,  0,  0,  0,  0,  5,-10,
            -20,-10,-10,-10,-10,-10,-10,-20
        },
        {
            0,  0,  0,  5,  5,  0,  0,  0,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            5, 10, 10, 10, 10, 10, 10,  5,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {
            -20,-10,-10, -5, -5,-10,-10,-20,
            -10,  0,  5,  0,  0,  0,  0,-10,
            -10,  5,  5,  5,  5,  5,  0,-10,
            0,  0,  5,  5,  5,  5,  0, -5,
            -5,  0,  5,  5,  5,  5,  0, -5,
            -10,  0,  5,  5,  5,  5,  0,-10,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -20,-10,-10, -5, -5,-10,-10,-20
        },
        {
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -30,-40,-40,-50,-50,-40,-40,-30,
            -20,-30,-30,-40,-40,-30,-30,-20,
            -10,-20,-20,-20,-20,-20,-20,-10,
            20, 20,  0,  0,  0,  0, 20, 20,
            20, 30, 10,  0,  0, 10, 30, 20
        }
    }
};

constexpr int psqt_eg[NUM_COLOURS][NUM_PIECE_TYPES][NUM_SQUARES] = {
    {
        {
            0,  0,  0,  0,  0,  0,  0,  0,
            50, 50, 50, 50, 50, 50, 50, 50,
            10, 10, 20, 30, 30, 20, 10, 10,
            5,  5, 10, 25, 25, 10,  5,  5,
            0,  0,  0, 20, 20,  0,  0,  0,
            5, -5,-10,  0,  0,-10, -5,  5,
            5, 10, 10,-20,-20, 10, 10,  5,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {
            -50,-40,-30,-30,-30,-30,-40,-50,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -30,  0, 10, 15, 15, 10,  0,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  0, 15, 20, 20, 15,  0,-30,
            -30,  5, 10, 15, 15, 10,  5,-30,
            -40,-20,  0,  5,  5,  0,-20,-40,
            -50,-40,-30,-30,-30,-30,-40,-50
        },
        {
            -20,-10,-10,-10,-10,-10,-10,-20,
            -10,  5,  0,  0,  0,  0,  5,-10,
            -10, 10, 10, 10, 10, 10, 10,-10,
            -10,  0, 10, 10, 10, 10,  0,-10,
            -10,  5,  5, 10, 10,  5,  5,-10,
            -10,  0,  5, 10, 10,  5,  0,-10,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -20,-10,-10,-10,-10,-10,-10,-20
        },
        {
            0,  0,  0,  0,  0,  0,  0,  0,
            5, 10, 10, 10, 10, 10, 10,  5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            0,  0,  0,  5,  5,  0,  0,  0
        },
        {
            -20,-10,-10, -5, -5,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5,  5,  5,  5,  0,-10,
            -5,  0,  5,  5,  5,  5,  0, -5,
            0,  0,  5,  5,  5,  5,  0, -5,
            -10,  5,  5,  5,  5,  5,  0,-10,
            -10,  0,  5,  0,  0,  0,  0,-10,
            -20,-10,-10, -5, -5,-10,-10,-20
        },
        {
            -50,-40,-30,-20,-20,-30,-40,-50,
            -30,-20,-10,  0,  0,-10,-20,-30,
            -30,-10, 20, 30, 30, 20,-10,-30,
            -30,-10, 30, 40, 40, 30,-10,-30,
            -30,-10, 30, 40, 40, 30,-10,-30,
            -30,-10, 20, 30, 30, 20,-10,-30,
            -30,-30,  0,  0,  0,  0,-30,-30,
            -50,-30,-30,-30,-30,-30,-30,-50
        }
    },
    {
        {
            0,  0,  0,  0,  0,  0,  0,  0,
            5, 10, 10,-20,-20, 10, 10,  5,
            5, -5,-10,  0,  0,-10, -5,  5,
            0,  0,  0, 20, 20,  0,  0,  0,
            5,  5, 10, 25, 25, 10,  5,  5,
            10, 10, 20, 30, 30, 20, 10, 10,
            50, 50, 50, 50, 50, 50, 50, 50,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {
            -50,-40,-30,-30,-30,-30,-40,-50,
            -40,-20,  0,  5,  5,  0,-20,-40,
            -30,  5, 10, 15, 15, 10,  5,-30,
            -30,  0, 15, 20, 20, 15,  0,-30,
            -30,  5, 15, 20, 20, 15,  5,-30,
            -30,  0, 10, 15, 15, 10,  0,-30,
            -40,-20,  0,  0,  0,  0,-20,-40,
            -50,-40,-30,-30,-30,-30,-40,-50
        },
        {
            -20,-10,-10,-10,-10,-10,-10,-20,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -10,  0,  5, 10, 10,  5,  0,-10,
            -10,  5,  5, 10, 10,  5,  5,-10,
            -10,  0, 10, 10, 10, 10,  0,-10,
            -10, 10, 10, 10, 10, 10, 10,-10,
            -10,  5,  0,  0,  0,  0,  5,-10,
            -20,-10,-10,-10,-10,-10,-10,-20
        },
        {
            0,  0,  0,  5,  5,  0,  0,  0,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            -5,  0,  0,  0,  0,  0,  0, -5,
            5, 10, 10, 10, 10, 10, 10,  5,
            0,  0,  0,  0,  0,  0,  0,  0
        },
        {
            -20,-10,-10, -5, -5,-10,-10,-20,
            -10,  0,  5,  0,  0,  0,  0,-10,
            -10,  5,  5,  5,  5,  5,  0,-10,
            0,  0,  5,  5,  5,  5,  0, -5,
            -5,  0,  5,  5,  5,  5,  0, -5,
            -10,  0,  5,  5,  5,  5,  0,-10,
            -10,  0,  0,  0,  0,  0,  0,-10,
            -20,-10,-10, -5, -5,-10,-10,-20
        },
        {
            -30,-30,  0,  0,  0,  0,-30,-30,
            -30,-10, 20, 30, 30, 20,-10,-30,
            -30,-10, 30, 40, 40, 30,-10,-30,
            -30,-10, 30, 40, 40, 30,-10,-30,
            -30,-10, 20, 30, 30, 20,-10,-30,
            -30,-20,-10,  0,  0,-10,-20,-30,
            -50,-40,-30,-20,-20,-30,-40,-50,
            -50,-30,-30,-30,-30,-30,-30,-50
        }
    }
};