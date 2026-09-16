#include "ai_manager.hpp"
#include "../pieces/pieces.hpp"
#include "../utility/logger.hpp"

#include <cstdlib>
#include <ctime>
#include <unordered_map>

constexpr int MATE_VALUE = INT_MAX - 1000; // not so close to INT_MAX to prevent overflow
constexpr Bitboard CENTRE(0x1818000000);
constexpr Bitboard EXTENDED_CENTRE(0x3c3c3c3c0000);
constexpr Bitboard KING_SAFETY(0xe7000000000000e7);

constexpr int CENTRE_PAWN_VAL = 25;
constexpr int EXTENDED_CENTRE_PAWN_VAL = 15;
constexpr int KING_SAFETY_VAL = 30;

constexpr uint16_t PAWN_VALUE = 100;
constexpr uint16_t KNIGHT_VALUE = 300;
constexpr uint16_t BISHOP_VALUE = 320;
constexpr uint16_t ROOK_VALUE = 500;
constexpr uint16_t QUEEN_VALUE = 900;

AIManager::AIManager(Colour ai_colour)
  : ai_colour(ai_colour),
    calls(0)
{
    LOG_TRACE;
    std::srand(std::time(nullptr));
}

// OLD GET_MOVE ------------------------------------------------------------
//
// Move AIManager::get_move(BoardManager& board_manager, int depth) {
//     LOG_TRACE;
//
//     calls = 0;
//     BoardState& bs = board_manager.get_bs();
//     tracked_array<Move, MAX_NUM_MOVES> moves = get_moves(bs.turn, bs);
//     tracked_array<Move, MAX_NUM_MOVES> valid_moves = board_manager.get_legal_moves(moves);
//
//     Move best = valid_moves[0];
//     int alpha = INT_MIN;
//     int beta = INT_MAX;
//
//     for(Move move : valid_moves) {
//         board_manager.make_move(move);
//         int score = min_max(board_manager, depth - 1, false, alpha, beta);
//         board_manager.undo_move();
//
//         if (score > alpha) {
//             alpha = score;
//             best = move;
//         }
//     }
//     return best;
// }

Move AIManager::get_move(BoardManager& board_manager, int depth) {
    LOG_TRACE;

    calls = 0;
    BoardState& bs = board_manager.get_bs();
    tracked_array<Move, MAX_NUM_MOVES> moves = get_moves(bs.turn, bs);
    tracked_array<Move, MAX_NUM_MOVES> valid_moves = board_manager.get_legal_moves(moves);

    Move best = valid_moves[0];

    int alpha = INT_MIN;
    int beta = INT_MAX;

    for(const Move& move : valid_moves) {
        board_manager.make_move(move);
        int score = min_max(board_manager, depth - 1, false, alpha, beta);
        board_manager.undo_move();

        if (score > alpha) {
            alpha = score;
            best = move;
        }
    }
    return best;
}

int AIManager::min_max(BoardManager& board_manager, int depth, bool is_maximising,  int alpha, int beta) {
    LOG_TRACE;
    BoardState& bs = board_manager.get_bs();

    if(depth == 0) return eval_board(bs);

    bool in_check = board_manager.is_in_check(bs.turn);
    tracked_array<Move, MAX_NUM_MOVES> moves = get_moves(bs.turn, bs);
    tracked_array<Move, MAX_NUM_MOVES> valid_moves = board_manager.get_legal_moves(moves, in_check);

    switch (board_manager.get_game_state(valid_moves, in_check)) {
        case GameState::CHECKMATE: return bs.turn == ai_colour ? -(MATE_VALUE + depth) : (MATE_VALUE + depth);
        case GameState::DRAW: return 0;
        default: break;
    }

    int best_score = is_maximising ? INT_MIN : INT_MAX;
    for (const Move& move : valid_moves) {
        board_manager.make_move(move);
        int score = min_max(board_manager, depth - 1, !is_maximising, alpha, beta);

        if (is_maximising) {
            best_score = std::max(best_score, score);
            alpha = std::max(alpha, score);
        } else {
            best_score = std::min(best_score, score);
            beta = std::min(beta, score);
        }

        board_manager.undo_move();

        if (beta <= alpha) break;
    }

    calls++;
    return best_score;
}

// simple score evaluator based upon how many pieces are on the board
// and what their associated value is
int AIManager::eval_board(const BoardState& bs) {
    int score = 0;

    const Bitboard& white_pawns = bs.get_bb(Colour::WHITE, PieceType::PAWN);
    const Bitboard& black_pawns = bs.get_bb(Colour::BLACK, PieceType::PAWN);

    // count pieces
    // pawns
    score += std::popcount(white_pawns) * PAWN_VALUE;
    score += std::popcount(black_pawns) * -PAWN_VALUE;

    // knights
    score += std::popcount(bs.get_bb(Colour::WHITE, PieceType::KNIGHT)) * KNIGHT_VALUE;
    score += std::popcount(bs.get_bb(Colour::BLACK, PieceType::KNIGHT)) * -KNIGHT_VALUE;

    // bishops
    score += std::popcount(bs.get_bb(Colour::WHITE, PieceType::BISHOP)) * BISHOP_VALUE;
    score += std::popcount(bs.get_bb(Colour::BLACK, PieceType::BISHOP)) * -BISHOP_VALUE;

    // rooks
    score += std::popcount(bs.get_bb(Colour::WHITE, PieceType::ROOK)) * BISHOP_VALUE;
    score += std::popcount(bs.get_bb(Colour::BLACK, PieceType::ROOK)) * -BISHOP_VALUE;

    // queens
    score += std::popcount(bs.get_bb(Colour::WHITE, PieceType::QUEEN)) * BISHOP_VALUE;
    score += std::popcount(bs.get_bb(Colour::BLACK, PieceType::QUEEN)) * -BISHOP_VALUE;

    // Positional pieces
    // pawns
    score += (std::popcount(white_pawns & CENTRE) - std::popcount(black_pawns & CENTRE)) * CENTRE_PAWN_VAL;
    score += (std::popcount(white_pawns & EXTENDED_CENTRE) - std::popcount(black_pawns & EXTENDED_CENTRE)) * EXTENDED_CENTRE_PAWN_VAL;

    // king safety
    const Bitboard& white_king = bs.get_bb(Colour::WHITE, PieceType::KING);
    const Bitboard& black_king = bs.get_bb(Colour::BLACK, PieceType::KING);
    score += (std::popcount(white_king & KING_SAFETY) - std::popcount(black_king & KING_SAFETY)) * KING_SAFETY_VAL;

    return ai_colour == Colour::WHITE ? score : -score; // flip so score is from AI's perspective
}