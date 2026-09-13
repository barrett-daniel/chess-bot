#pragma once

#include <array>
#include <iostream>
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <vector>
#include <boost/container/static_vector.hpp>

#ifdef NDEBUG
    const int FPS = 30;
    constexpr int AI_BASE_DEPTH = 5;
    #define ASSERT(expr, msg) ((void)0)
#else
    const int FPS = 5;
    constexpr int AI_BASE_DEPTH = 3;
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
enum class Colour : uint8_t {
    WHITE, BLACK
};

inline Colour operator!(Colour c) {
    return c == Colour::WHITE ? Colour::BLACK : Colour::WHITE;
}

enum class PieceType : uint8_t {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE
};

enum class GameState : uint8_t {
    CHECK,
    CHECKMATE,
    STALEMATE,
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
        Square en_passant_square;
        tracked_array<Square, BOARD_SIZE> pinned_sqs; // max number of pinned pieces is 8 as there are only 8 directions

        uint16_t full_move_count;
        uint8_t half_move_count; // half move logic for 50-move draw rule is not yet implemented

        BoardState()
            : w_pawns(0), w_knights(0), w_bishops(0), w_rooks(0), w_queens(0), w_king(0),
            b_pawns(0), b_knights(0), b_bishops(0), b_rooks(0), b_queens(0), b_king(0),
            w_pieces(0), b_pieces(0), all_pieces(0), en_passant_square(NULL_SQ)
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
