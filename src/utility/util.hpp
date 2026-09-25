#pragma once

#include <array>
#include <iostream>
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <format>
#include <utility>
#include <vector>
#include <boost/container/static_vector.hpp>

#ifdef NDEBUG
    const int FPS = 30;
    constexpr int AI_MAX_DEPTH = 50;
    constexpr int AI_SEARCH_TIME_MS = 2000;
    #define ASSERT(expr, msg) ((void)0)
#else
    const int FPS = 5;
    constexpr int AI_MAX_DEPTH = 25;
    constexpr int AI_SEARCH_TIME_MS = 2000;
    #define ASSERT(expr, msg) \
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

// FEN
constexpr const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr const char* BLANK_FEN = "8/8/8/8/8/8/8/8";
constexpr int FEN_NUM_SECTIONS = 2;
constexpr int FEN_BOARD_CONFIG= 0;
constexpr int FEN_TURN_CONFIG = 1;
constexpr int FEN_CASTLING_CONFIG = 2;
constexpr int FEN_EN_PASSANT_CONFIG = 3;
constexpr int FEN_HALFMOVE_CONFIG = 4;
constexpr int FEN_FULLMOVE_CONFIG = 5;

// Rendering 
constexpr const char* WINDOW_NAME = "Chess";
constexpr int CELL_SIZE = 128;
constexpr int BOARD_SIZE = 8;
constexpr int WINDOW_DIM = BOARD_SIZE * CELL_SIZE;

// Bitboard & Board
typedef uint64_t Bitboard;
typedef uint8_t Square;
constexpr Square NULL_SQ = 64;
constexpr uint8_t NUM_SQUARES = 64;

extern Bitboard KNIGHT_ATTACKS[BOARD_SIZE * BOARD_SIZE];
extern Bitboard PAWN_ATTACKS_W[BOARD_SIZE * BOARD_SIZE];
extern Bitboard PAWN_ATTACKS_B[BOARD_SIZE * BOARD_SIZE];
extern Bitboard KING_ATTACKS[BOARD_SIZE * BOARD_SIZE];

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

// Sqs
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

// Castling
constexpr uint8_t W_QUEEN_CASTLE_MASK = 0b00001000;
constexpr uint8_t W_KING_CASTLE_MASK  = 0b00000100;
constexpr uint8_t B_QUEEN_CASTLE_MASK = 0b00000010;
constexpr uint8_t B_KING_CASTLE_MASK  = 0b00000001;

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

// Move constants
extern std::vector<std::pair<int, Bitboard>> ROOK_DIRECTIONS;
extern std::vector<std::pair<int, Bitboard>> BISHOP_DIRECTIONS;
extern std::vector<std::pair<int, Bitboard>> KING_DIRECTIONS;
extern std::vector<std::pair<int, Bitboard>> KNIGHT_DIRECTIONS;
constexpr uint8_t MAX_NUM_MOVES = 218; // source: https://lichess.org/@/Tobs40/blog/why-a-reachable-position-can-have-at-most-218-playable-moves/a5xdxeqs

// Data types
constexpr uint8_t NUM_COLOURS = 2;
enum class Colour : uint8_t {
    WHITE, BLACK
};
constexpr int colour_sign[NUM_COLOURS] = {
    1, // white
    -1 // black
};

inline Colour operator!(Colour c) {
    return c == Colour::WHITE ? Colour::BLACK : Colour::WHITE;
}

constexpr uint8_t NUM_PIECE_TYPES = 6; // note: 7 == NONE
enum class PieceType : uint8_t {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE
};
constexpr uint8_t PT_PAWN_IDX = std::to_underlying(PieceType::PAWN);
constexpr uint8_t PT_KNIGHT_IDX = std::to_underlying(PieceType::KNIGHT);
constexpr uint8_t PT_BISHOP_IDX = std::to_underlying(PieceType::BISHOP);
constexpr uint8_t PT_ROOK_IDX = std::to_underlying(PieceType::ROOK);
constexpr uint8_t PT_QUEEN_IDX = std::to_underlying(PieceType::QUEEN);
constexpr uint8_t PT_KING_IDX = std::to_underlying(PieceType::KING);
constexpr uint8_t PT_NONE_IDX = std::to_underlying(PieceType::NONE);

constexpr uint8_t NUM_GAME_STATES = 4;
enum class GameState : uint8_t {
    CHECK,
    CHECKMATE,
    DRAW,
    UNFINISHED
};

// we define a compile-time overload of tracked_array to be a vector in debug
// and a boost::static vector in release mode.
//
// the static vector yields far better performance, but is somewhat of a black box
// when using a debugger, hence the need to use a standard vector.
#ifdef NDEBUG
    template <typename T, size_t N>
    using tracked_array = boost::container::static_vector<T, N>; // alias
#else
    template <typename T, size_t N>
    using tracked_array = std::vector<T>;
#endif

std::string to_string_piece(PieceType piece);
std::string to_string_sq(Square sq);
class Move {
    public:
        Square from;
        Square to;
        PieceType moved_piece;

        PieceType promotion_piece;
        bool is_en_passant;
        bool q_castle;
        bool k_castle;

        bool operator==(const Move&) const = default;

        Move()
        : from(NULL_SQ),
            to(NULL_SQ),
            moved_piece(PieceType::NONE),
            promotion_piece(PieceType::NONE),
            is_en_passant(false),
            q_castle(false),
            k_castle(false)
            {}

        Move(Square from, Square to, PieceType moved_piece)
        : from(from), 
            to(to),
            moved_piece(moved_piece), 
            promotion_piece(PieceType::NONE),
            is_en_passant(false), 
            q_castle(false),
            k_castle(false)
        {}

        std::string to_string() const {
            std::string s = to_string_sq(from) + to_string_sq(to);
            if (promotion_piece != PieceType::NONE) {
                switch (promotion_piece) {
                    case PieceType::QUEEN:  s += 'q'; break;
                    case PieceType::ROOK:   s += 'r'; break;
                    case PieceType::BISHOP: s += 'b'; break;
                    case PieceType::KNIGHT: s += 'n'; break;
                    default: break;
                }
            }
            return s;
        }
};

constexpr int PAWN_VALUE   = 100;
constexpr int KNIGHT_VALUE = 300;
constexpr int BISHOP_VALUE = 310;
constexpr int ROOK_VALUE   = 500;
constexpr int QUEEN_VALUE  = 900;
constexpr int KING_VALUE   = 0;

constexpr int32_t piece_value[NUM_PIECE_TYPES] = {
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

constexpr uint8_t phase_value[NUM_PIECE_TYPES] = {
    PAWN_PHASE,
    KNIGHT_PHASE,
    BISHOP_PHASE,
    ROOK_PHASE,
    QUEEN_PHASE,
    KING_PHASE
};

/*************************************************

                Piece Square Tables

*************************************************/

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

// max possible material on the board is known, converted to phase values https://lichess.org/study/AW5mab1c
constexpr uint8_t MAX_PHASE   = (18 * QUEEN_PHASE) + (2 * ROOK_PHASE) * (2 * KING_PHASE);
constexpr uint8_t START_PHASE = 24;
struct EvaluationData {
    int32_t score_mg;
    int32_t score_eg;
    uint8_t phase;

    /**
     *
     * @return the interpolated score between middle and endgame scores based on the phase (heuristic for amount of material on board)
     */
    [[nodiscard]] int32_t get_score() const {
        ASSERT(phase < MAX_PHASE, "Phase exceeds maximum possible phase value");
        uint8_t clamped_phase = std::min(phase, START_PHASE);
        return (score_mg * clamped_phase + score_eg * (START_PHASE - clamped_phase)) / START_PHASE;
    }

    bool operator ==(const EvaluationData&) const = default;

    std::string to_string() const {
        return std::format("(score_mg = {}, score_eg = {}, phase = {})", score_mg, score_eg, phase);
    }

    void remove_piece_score(uint8_t piece_type, uint8_t colour, Square sq) {
        const int sign = colour_sign[colour];

        score_mg -= piece_value[piece_type] * sign;
        score_eg -= piece_value[piece_type] * sign;
        score_mg -= psqt_mg[colour][piece_type][sq] * sign;
        score_eg -= psqt_eg[colour][piece_type][sq] * sign;
        phase -= phase_value[piece_type];
    }

    void remove_piece_score(PieceType piece_type, Colour colour, Square sq) {
        remove_piece_score(std::to_underlying(piece_type), std::to_underlying(colour), sq);
    }

    void add_piece_score(uint8_t piece_type, uint8_t colour, Square sq) {
        const int sign = colour_sign[colour];

        score_mg += piece_value[piece_type] * sign;
        score_eg += piece_value[piece_type] * sign;
        score_mg += psqt_mg[colour][piece_type][sq] * sign;
        score_eg += psqt_eg[colour][piece_type][sq] * sign;
        phase += phase_value[piece_type];
    }

    void add_piece_score(PieceType piece_type, Colour colour, Square sq) {
        add_piece_score(std::to_underlying(piece_type), std::to_underlying(colour), sq);
    }

    void process_move(uint8_t piece_type, uint8_t colour, Square move_from, Square move_to) {
        const int sign = colour_sign[colour];

        score_mg -= psqt_mg[colour][piece_type][move_from] * sign;
        score_mg += psqt_mg[colour][piece_type][move_to]   * sign;
        score_eg -= psqt_eg[colour][piece_type][move_from] * sign;
        score_eg += psqt_eg[colour][piece_type][move_to]   * sign;
    }

    void process_move(PieceType piece_type, Colour colour, Square move_from, Square move_to) {
        process_move(std::to_underlying(piece_type), std::to_underlying(colour), move_from, move_to);
    }
};

inline std::ostream& operator<<(std::ostream& os, const EvaluationData& ed) {
    os << "EvaluationData{"
       << "score_mg=" << ed.score_mg
       << ", score_eg=" << ed.score_eg
       << ", phase=" << static_cast<int>(ed.phase)  // uint8_t would print as a char otherwise
       << "}";
    return os;
}

constexpr int NUM_PIECE_BBS = 12;
class BoardState {
    public:
        // Piece bitboards are stored as an array of the form:
        // [white pawn, white knight, ... all white pieces,
        //  black pawn, black knight, ... all black pieces]
        // Based on the casted index of the PieceType and Colour enums
        // get_bb allows a clean interface to index into array and fetch
        // the associated bitboard based on the desired PieceType and Colour
        std::array<Bitboard, 14> bbs;
        Bitboard all_pieces;

        Colour turn;
        uint8_t castling_rights;
        Square en_passant_square;
        tracked_array<Square, BOARD_SIZE> pinned_sqs; // max number of pinned pieces is 8 as there are only 8 directions

        uint16_t full_move_count;
        uint8_t half_move_count; // half move logic for 50-move draw rule is not yet implemented

        uint64_t hash;
        EvaluationData edata;

        BoardState() : bbs({}), all_pieces(0), turn(Colour::WHITE), castling_rights(0), en_passant_square(NULL_SQ), full_move_count(0), half_move_count(0), hash(0) {}

        inline Bitboard& get_bb(Colour side, PieceType type) {
            return bbs[static_cast<uint8_t>(side) * 7 + static_cast<uint8_t>(type)];
        }

        inline Bitboard get_bb(Colour side, PieceType type) const {
            return bbs[static_cast<uint8_t>(side) * 7 + static_cast<uint8_t>(type)];
        }

        std::string to_string() const {
            // [white][black] piece characters, indexed by PieceType value (0-5)
            static constexpr std::array<std::array<char, 6>, 2> piece_chars = {{
                {'P', 'N', 'B', 'R', 'Q', 'K'},  // white
                {'p', 'n', 'b', 'r', 'q', 'k'}   // black
            }};

            std::string out;
            out.reserve(8 * 8 * 2 + 8 * 4 + 20);

            out += "  a b c d e f g h\n";
            for (int rank = 7; rank >= 0; --rank) {
                out += std::to_string(rank + 1);
                out += ' ';

                for (int file = 0; file < 8; ++file) {
                    int sq = rank * 8 + file;
                    Bitboard mask = static_cast<Bitboard>(1) << sq;
                    char piece_char = '.';

                    for (int colour = 0; colour < 2 && piece_char == '.'; ++colour) {
                        for (int pt = 0; pt < 6; ++pt) {
                            Bitboard bb = get_bb(static_cast<Colour>(colour), static_cast<PieceType>(pt));
                            if (bb & mask) {
                                piece_char = piece_chars[colour][pt];
                                break;
                            }
                        }
                    }

                    out += piece_char;
                    out += ' ';
                }

                out += ' ';
                out += std::to_string(rank + 1);
                out += '\n';
            }
            out += "  a b c d e f g h\n";

            return out;
        }

        std::string to_fen() const {
            std::string fen;
            fen.reserve(80);

            // --- 1. Piece placement ---
            for (int rank = 7; rank >= 0; --rank) {
                int empty_count = 0;
                for (int file = 0; file < 8; ++file) {
                    int sq = rank * 8 + file;
                    Bitboard mask = static_cast<Bitboard>(1) << sq;
                    char piece_char = '\0';

                    for (int colour = 0; colour < 2 && piece_char == '\0'; ++colour) {
                        for (int pt = 0; pt < 6; ++pt) {
                            Bitboard bb = get_bb(static_cast<Colour>(colour), static_cast<PieceType>(pt));
                            if (bb & mask) {
                                static constexpr std::array<std::array<char, 6>, 2> piece_chars = {{
                                    {'P', 'N', 'B', 'R', 'Q', 'K'},  // white
                                    {'p', 'n', 'b', 'r', 'q', 'k'}   // black
                                }};
                                piece_char = piece_chars[colour][pt];
                                break;
                            }
                        }
                    }

                    if (piece_char == '\0') {
                        ++empty_count;
                    } else {
                        if (empty_count > 0) {
                            fen += std::to_string(empty_count);
                            empty_count = 0;
                        }
                        fen += piece_char;
                    }
                }
                if (empty_count > 0) {
                    fen += std::to_string(empty_count);
                }
                if (rank > 0) {
                    fen += '/';
                }
            }

            // --- 2. Active colour ---
            fen += ' ';
            fen += (turn == Colour::WHITE) ? 'w' : 'b';

            // --- 3. Castling rights ---
            // ASSUMPTION: bit0 = white kingside (K), bit1 = white queenside (Q),
            // bit2 = black kingside (k), bit3 = black queenside (q).
            // Adjust the mask/order below if your actual bit layout differs.
            fen += ' ';
            std::string castling;
            if (castling_rights & W_KING_CASTLE_MASK)  castling += 'K';
            if (castling_rights & W_QUEEN_CASTLE_MASK) castling += 'Q';
            if (castling_rights & B_KING_CASTLE_MASK)  castling += 'k';
            if (castling_rights & B_QUEEN_CASTLE_MASK) castling += 'q';
            fen += castling.empty() ? "-" : castling;

            // --- 4. En passant target square ---
            // ASSUMPTION: Square is an integer-like type where value == rank*8 + file
            // (matching the indexing used in to_string()/get_bb loops above),
            // and NULL_SQ is the sentinel for "no en passant square".
            fen += ' ';
            if (en_passant_square == NULL_SQ) {
                fen += '-';
            } else {
                int sq = static_cast<int>(en_passant_square);
                int file = sq % 8;
                int rank = sq / 8;
                fen += static_cast<char>('a' + file);
                fen += static_cast<char>('1' + rank);
            }

            // --- 5. Halfmove clock ---
            fen += ' ';
            fen += std::to_string(static_cast<int>(half_move_count));

            // --- 6. Fullmove number ---
            // ASSUMPTION: full_move_count already follows FEN convention (starts at 1
            // for the first move). If your engine starts it at 0 internally, you may
            // want `std::to_string(full_move_count + 1)` instead.
            fen += ' ';
            fen += std::to_string(full_move_count);

            return fen;
        }
};

std::string to_string_piece_colour(PieceType piece, Colour side);
std::string to_string_colour(Colour colour);
std::string to_string_coord(int coord);
std::string to_string(const Bitboard& board);
bool contains(const std::vector<Move>& moves, const Move& move);
Bitboard shift_dir(Bitboard bb, int dir);
void log_bitboard(const Bitboard& bb);
bool is_game_over(GameState state);

constexpr inline void set_bit(Bitboard& bb, Square pos) {
    bb |= (1ULL << pos);
}

constexpr inline void clear_bit(Bitboard& bb, Square pos) {
    bb &= ~(1ULL << pos);
}

constexpr inline bool is_bit(const Bitboard& bb, Square pos) {
    return (bb & (1ULL << pos)) != 0;
}

// pops the LSB of a bitboard -> return index of bit
constexpr inline Square pop_lsb(Bitboard &bb) {
    Square index = std::countr_zero(bb);
    bb &= bb - 1;
    return index;
}

constexpr inline bool is_capture(const Move& move, const BoardState& bs) {
    const Bitboard& enemy_pieces = bs.get_bb(!bs.turn, PieceType::NONE);
    return is_bit(enemy_pieces, move.to);
}

template<typename T>
std::vector<T> combine(const std::vector<T>& v) {
    return v;
}

template<typename T, typename... Vectors>
std::vector<T> combine(const std::vector<T>& first, const Vectors&... rest) {
    std::vector<T> result;
    result.reserve(first.size() + (rest.size() + ...));
    result.insert(result.end(), first.begin(), first.end());
    (result.insert(result.end(), rest.begin(), rest.end()), ...);
    return result;
}
