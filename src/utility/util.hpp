#pragma once

#include <array>
#include <iostream>
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <bitset>
#include <cstdint>
#include <vector>

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

#ifdef NDEBUG
    const int FPS = 30;
    constexpr int AI_BASE_DEPTH = 5;
#else
    const int FPS = 5;
    constexpr int AI_BASE_DEPTH = 3;
#endif

// FEN
constexpr const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";
constexpr const char* BLANK_FEN = "8/8/8/8/8/8/8/8";
constexpr int FEN_NUM_SECTIONS = 2;
constexpr int FEN_BOARD_CONFIG= 0;
constexpr int FEN_TURN_CONFIG= 1;

// Rendering 
constexpr const char* WINDOW_NAME = "Chess";
constexpr int CELL_SIZE = 128;
constexpr int BOARD_SIZE = 8;
constexpr int WINDOW_DIM = BOARD_SIZE * CELL_SIZE;

// Bitboard & Board
typedef uint64_t Bitboard;

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

// Castling
constexpr uint8_t W_QUEEN_CASTLE_MASK = 0b00001000;
constexpr uint8_t W_KING_CASTLE_MASK  = 0b00000100;
constexpr uint8_t B_QUEEN_CASTLE_MASK = 0b00000010;
constexpr uint8_t B_KING_CASTLE_MASK  = 0b00000001;

constexpr int W_KING_START_POS = 60;
constexpr int B_KING_START_POS = 4;

constexpr int W_QUEEN_ROOK_START_POS = 56;
constexpr int W_KING_ROOK_START_POS  = 63;
constexpr int B_QUEEN_ROOK_START_POS = 0;
constexpr int B_KING_ROOK_START_POS  = 7;

constexpr int W_QUEEN_ROOK_CASTLE_TO  = 59;
constexpr int W_KING_ROOK_CASTLE_TO   = 61;
constexpr int B_QUEEN_ROOK_CASTLE_TO  = 3;
constexpr int B_KING_ROOK_CASTLE_TO   = 5;

constexpr int W_KING_CASTLE_TO = 62;
constexpr int W_QUEEN_CASTLE_TO = 58;
constexpr int B_KING_CASTLE_TO = 6;
constexpr int B_QUEEN_CASTLE_TO = 2;

// Sqs - for testing
constexpr int A8 = 0;
constexpr int B8 = 1;
constexpr int C8 = 2;
constexpr int D8 = 3;
constexpr int E8 = 4;
constexpr int F8 = 5;
constexpr int G8 = 6;
constexpr int H8 = 7;

constexpr int A7 = 8;
constexpr int B7 = 9;
constexpr int C7 = 10;
constexpr int D7 = 11;
constexpr int E7 = 12;
constexpr int F7 = 13;
constexpr int G7 = 14;
constexpr int H7 = 15;

constexpr int A6 = 16;
constexpr int B6 = 17;
constexpr int C6 = 18;
constexpr int D6 = 19;
constexpr int E6 = 20;
constexpr int F6 = 21;
constexpr int G6 = 22;
constexpr int H6 = 23;

constexpr int A5 = 24;
constexpr int B5 = 25;
constexpr int C5 = 26;
constexpr int D5 = 27;
constexpr int E5 = 28;
constexpr int F5 = 29;
constexpr int G5 = 30;
constexpr int H5 = 31;

constexpr int A4 = 32;
constexpr int B4 = 33;
constexpr int C4 = 34;
constexpr int D4 = 35;
constexpr int E4 = 36;
constexpr int F4 = 37;
constexpr int G4 = 38;
constexpr int H4 = 39;

constexpr int A3 = 40;
constexpr int B3 = 41;
constexpr int C3 = 42;
constexpr int D3 = 43;
constexpr int E3 = 44;
constexpr int F3 = 45;
constexpr int G3 = 46;
constexpr int H3 = 47;

constexpr int A2 = 48;
constexpr int B2 = 49;
constexpr int C2 = 50;
constexpr int D2 = 51;
constexpr int E2 = 52;
constexpr int F2 = 53;
constexpr int G2 = 54;
constexpr int H2 = 55;

constexpr int A1 = 56;
constexpr int B1 = 57;
constexpr int C1 = 58;
constexpr int D1 = 59;
constexpr int E1 = 60;
constexpr int F1 = 61;
constexpr int G1 = 62;
constexpr int H1 = 63;

// Move constants
extern std::vector<std::pair<int, Bitboard>> ROOK_DIRECTIONS;
extern std::vector<std::pair<int, Bitboard>> BISHOP_DIRECTIONS;
extern std::vector<std::pair<int, Bitboard>> KING_DIRECTIONS;
extern std::vector<std::pair<int, Bitboard>> KNIGHT_DIRECTIONS;

// Data types
enum Colour {
    WHITE, BLACK
};

inline Colour operator!(Colour c) {
    return c == WHITE ? BLACK : WHITE;
}

enum AiType {
    RANDOM,
    FIRST,
    SMART
};

enum PieceType {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE
};

class Move {
    public:
        int from;
        int to;
        PieceType moved_piece;

        PieceType promotion_piece;
        bool is_en_passant;
        bool q_castle;
        bool k_castle;

        bool operator==(const Move&) const = default;

        Move(int from, int to, PieceType moved_piece)
        : from(from), 
            to(to),
            moved_piece(moved_piece), 
            promotion_piece(NONE),
            is_en_passant(false), 
            q_castle(false),
            k_castle(false)
        {}
};

constexpr int NUM_PIECE_BBS = 12;
class BoardState {
    public:
        Bitboard w_pawns, w_knights, w_bishops, w_rooks, w_queens, w_king;
        Bitboard b_pawns, b_knights, b_bishops, b_rooks, b_queens, b_king;
        
        Bitboard w_pieces;
        Bitboard b_pieces;
        Bitboard all_pieces;   

        Colour turn;
        uint8_t castling_rights;
        int en_passant_square;
        std::vector<int> pinned_sqs;

        BoardState()
            : w_pawns(0), w_knights(0), w_bishops(0), w_rooks(0), w_queens(0), w_king(0),
            b_pawns(0), b_knights(0), b_bishops(0), b_rooks(0), b_queens(0), b_king(0),
            w_pieces(0), b_pieces(0), all_pieces(0)
        {}

        Bitboard& get_bb(Colour side, PieceType type) {
            switch (side) {
                case Colour::WHITE:
                    switch (type) {
                        case PieceType::PAWN:   return w_pawns;
                        case PieceType::KNIGHT: return w_knights;
                        case PieceType::BISHOP: return w_bishops;
                        case PieceType::ROOK:   return w_rooks;
                        case PieceType::QUEEN:  return w_queens;
                        case PieceType::KING:   return w_king;
                        case PieceType::NONE:   return w_pieces;
                    }
                    break;
                case Colour::BLACK:
                    switch (type) {
                        case PieceType::PAWN:   return b_pawns;
                        case PieceType::KNIGHT: return b_knights;
                        case PieceType::BISHOP: return b_bishops;
                        case PieceType::ROOK:   return b_rooks;
                        case PieceType::QUEEN:  return b_queens;
                        case PieceType::KING:   return b_king;
                        case PieceType::NONE:   return b_pieces;
                    }
                    break;
            }
            throw std::invalid_argument("Invalid colour or piece type");
        }

        std::array<std::reference_wrapper<Bitboard>, NUM_PIECE_BBS> piece_bbs() {
            return {
                std::ref(w_pawns), std::ref(w_knights), std::ref(w_bishops),
                std::ref(w_rooks), std::ref(w_queens), std::ref(w_king),
                std::ref(b_pawns), std::ref(b_knights), std::ref(b_bishops),
                std::ref(b_rooks), std::ref(b_queens), std::ref(b_king)
            };
        }
};

std::string to_string(Colour colour);
std::string to_string_coord(int coord);
std::string to_string(const Bitboard& board);

constexpr inline void set_bit(Bitboard& bb, int pos) {
    bb |= (1ULL << pos);
}

constexpr inline void clear_bit(Bitboard& bb, int pos) {
    bb &= ~(1ULL << pos);
}

constexpr inline bool is_bit(const Bitboard& bb, int pos) {
    return (bb & (1ULL << pos)) != 0;
}

// pops the LSB of a bitboard -> return index of bit
constexpr inline int pop_lsb(uint64_t &bb) {
    int index = __builtin_ctzll(bb);
    bb &= bb - 1;
    return index;
}

bool contains(const std::vector<Move>& moves, const Move& move);
Bitboard shift_dir(Bitboard bb, int dir);
void log_bitboard(const Bitboard& bb);

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
