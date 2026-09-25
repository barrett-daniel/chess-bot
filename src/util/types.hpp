#pragma once
#include <ostream>
#include <sstream>
#include <utility>
#include <vector>
#include <boost/container/static_vector.hpp>

/* ***********************************************

            Fixed-Width Abbreviations

*********************************************** */

// Unsigned types

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// Signed types

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

/* ***********************************************

                        Colour

*********************************************** */

constexpr u8 NUM_COLOURS = 2;

enum class Colour : u8 {
    WHITE = 0,
    BLACK = 1
};

/**
 * Indexable array used to multiply some value and negate such as needed if the colour is BLACK
 * (as values are by default determined from white's POV).
 */
constexpr int colour_sign[NUM_COLOURS] = {
    1, // white
    -1 // black
};

/**
 * Flips the colour from `BLACK` <-> `WHITE` and vice versa
 *
 * @param c colour to flip
 * @return the inverse colour
 */
inline Colour operator!(Colour c) {
    return c == Colour::WHITE ? Colour::BLACK : Colour::WHITE;
}

inline std::string to_string(Colour colour) {
    switch (colour) {
        case Colour::WHITE: return "WHITE";
        case Colour::BLACK: return "BLACK";
    }
    std::unreachable();
}

/* ***********************************************

                    Piece Types

*********************************************** */

constexpr u8 NUM_PIECE_TYPES = 6; // NOTE: excludes `NONE` which `==7`

enum class PieceType : u8 {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5,
    NONE = 6
};

inline std::string to_string(PieceType piece) {
    switch (piece) {
        case PieceType::PAWN:   return "";
        case PieceType::KNIGHT: return "n";
        case PieceType::BISHOP: return "b";
        case PieceType::ROOK:   return "r";
        case PieceType::QUEEN:  return "q";
        case PieceType::KING:   return "k";
        case PieceType::NONE:   return "NONE";
    }
    std::unreachable();
}

// Quick access variables to the underlying int value of each piece type enum value.
// Used for when indexing an array based on the `NUM_PIECE_TYPES`.

constexpr u8 PT_PAWN_IDX = std::to_underlying(PieceType::PAWN);
constexpr u8 PT_KNIGHT_IDX = std::to_underlying(PieceType::KNIGHT);
constexpr u8 PT_BISHOP_IDX = std::to_underlying(PieceType::BISHOP);
constexpr u8 PT_ROOK_IDX = std::to_underlying(PieceType::ROOK);
constexpr u8 PT_QUEEN_IDX = std::to_underlying(PieceType::QUEEN);
constexpr u8 PT_KING_IDX = std::to_underlying(PieceType::KING);
constexpr u8 PT_NONE_IDX = std::to_underlying(PieceType::NONE);

/* ***********************************************

                    Static Vector

*********************************************** */

// we define a compile-time overload of tracked_array to be a vector in debug
// and as boost::static vector in release mode.
//
// the static vector yields far better performance, but is somewhat of a black box
// when using a debugger, hence the need to use a standard vector when debugging

#ifdef NDEBUG
    template <typename T, size_t N>
    using tracked_array = boost::container::static_vector<T, N>;
#else
    template <typename T, size_t N>
    using tracked_array = std::vector<T>;
#endif

/* ***********************************************

                    Game State

*********************************************** */

constexpr u8 NUM_GAME_STATES = 4;

enum class GameState : u8 {
    CHECK,
    CHECKMATE,
    DRAW,
    UNFINISHED
};

/* ***********************************************

                        Square

*********************************************** */

typedef u8 Square;

inline std::string to_string_sq(Square sq) {
    char file = 'a' + (sq % 8);
    char rank = '1' + (sq / 8);
    return std::string(1, file) + std::string(1, rank);
}


/* ***********************************************

                        Bitboard

*********************************************** */

typedef u64 Bitboard;

inline std::string to_string_bb(const Bitboard& board) {
    {
        std::stringstream ss;
        for (int rank = 0; rank < 8; rank++) {
            ss << rank + 1 << "  ";
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                ss << ((board & (1ULL << square)) != 0 ? '1' : '.') << " ";
            }
            ss << std::endl;
        }

        ss << "   A B C D E F G H\n";
        return ss.str();
    }
}

