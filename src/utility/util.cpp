#include "util.hpp"

#include <algorithm>

#include "logger.hpp"

// precomputed bitboards of: for the given index (which is a square on the board), 1 if a piece of the
// given type is able to attack the square index, 0 if not.
Bitboard KNIGHT_ATTACKS[64] = {0};
Bitboard PAWN_ATTACKS_W[64] = {0};
Bitboard PAWN_ATTACKS_B[64] = {0};
Bitboard KING_ATTACKS[64]   = {0};

std::vector<std::pair<int, Bitboard>> ROOK_DIRECTIONS = {
    {-8, EMPTY},    // N
    { 8, EMPTY},    // S
    { 1, FILE_H},   // E
    {-1, FILE_A},   // W
};

std::vector<std::pair<int, Bitboard>> BISHOP_DIRECTIONS = {
    {-7, FILE_H},  // NE
    { 9, FILE_H},  // SE
    { 7, FILE_A},  // SW
    {-9, FILE_A},  // NW
};

std::vector<std::pair<int, Bitboard>> KING_DIRECTIONS = {
    {-8, EMPTY},    // N
    {-7,  FILE_H},  // NE
    {1,  FILE_H},   // E
    {9,  FILE_H},   // SE
    {8,  EMPTY},    // S
    {7,   FILE_A},  // SW
    {-1, FILE_A},   // W
    {-9, FILE_A},   // NW
};

std::vector<std::pair<int, Bitboard>> KNIGHT_DIRECTIONS = {
    {-15, FILE_H},              // NNE
    {-6,  FILE_G | FILE_H},     // NEE
    {10,  FILE_G | FILE_H},     // SEE
    {17,  FILE_H},              // SSE
    {15,  FILE_A},              // SSW
    {6,   FILE_A | FILE_B},     // SWW
    {-10, FILE_A | FILE_B},     // NWW
    {-17, FILE_A},              // NNW
};

bool contains(const std::vector<Move>& moves, const Move& move) {
    bool flag = false;
    for(auto m : moves) {
        if(m == move) flag = true;
    }
    return flag;
}

std::string to_string_piece(PieceType piece) {
    switch (piece) {
        case PAWN:   return "";
        case KNIGHT: return "n";
        case BISHOP: return "b";
        case ROOK:   return "r";
        case QUEEN:  return "q";
        case KING:   return "k";
        case NONE:   return "NONE";
    }
}

std::string to_string_piece_colour(PieceType piece, Colour side) {
    std::string piece_str = to_string_piece(piece);

    if (side == WHITE) {
        std::transform(piece_str.begin(), piece_str.end(), piece_str.begin(), ::toupper);
    }

    return piece_str;
}

std::string to_string_colour(Colour colour) {
    switch (colour) {
        case Colour::WHITE: return "WHITE";
        case Colour::BLACK: return "BLACK";
    }
}

std::string to_string_coord(int coord) {
    if(coord < 0 | coord >= BOARD_SIZE * BOARD_SIZE) return "{invalid coordinate}";
    
    int i = coord % BOARD_SIZE;
    int j = coord / BOARD_SIZE;

    std::stringstream ss;
    ss << "{";
    ss << i;
    ss << ", ";
    ss << j;
    ss << "}";
    return ss.str();
}

std::string to_string(const Bitboard& bb) {
    std::stringstream ss;
    for (int rank = 0; rank < 8; rank++) {
        ss << rank + 1 << "  ";
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            ss << (is_bit(bb, square) ? '1' : '.') << " ";
        }
        ss << std::endl;
    }

    ss << "   A B C D E F G H\n";
    return ss.str();
}

std::string to_string_sq(int sq) {
    char file = 'a' + (sq % 8);
    char rank = '0' + (8 - (sq / 8));
    return std::string(1, file) + std::string(1, rank);
}

Bitboard shift_dir(Bitboard bb, int dir) {
    return dir > 0 ? bb << dir : bb >> (dir * -1); 
};

void log_bitboard(const Bitboard& bb) {
    std::string bb_str = to_string(bb);

    std::istringstream iss(bb_str);
    for(std::string line; std::getline(iss, line); ) {
        LOG(line);
    }
}

bool is_game_over(GameState state) {
    switch (state) {
        case CHECKMATE:
        case STALEMATE: return true;
        case UNFINISHED:
        case CHECK: return false;
    }
}