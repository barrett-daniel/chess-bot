#include "ai_manager.hpp"
#include "../pieces/pieces.hpp"
#include "../utility/logger.hpp"

#include <cstdlib>
#include <ctime>
#include <unordered_map>

#define COUNT_BITS(expr) __builtin_popcountll(expr)

constexpr int MATE_VALUE = INT_MAX - 1000; // not so close to INT_MAX to prevent overflow
constexpr Bitboard CENTRE(0x1818000000);
constexpr Bitboard EXTENDED_CENTRE(0x3c3c3c3c0000);
constexpr Bitboard KING_SAFETY(0xe7000000000000e7);

constexpr int CENTRE_PAWN_VAL = 25;
constexpr int EXTENDED_CENTRE_PAWN_VAL = 15;
constexpr int KING_SAFETY_VAL = 30;

std::unordered_map<PieceType, int> PIECE_MAP = {
    {PieceType::PAWN, 100},
    {PieceType::KNIGHT, 300},
    {PieceType::BISHOP, 320},
    {PieceType::ROOK, 500},
    {PieceType::QUEEN, 900}
};

AIManager::AIManager(Colour ai_colour)
  : ai_colour(ai_colour),
    calls(0)
{
    LOG_TRACE;
    std::srand(std::time(nullptr));
}

Move AIManager::get_move(BoardManager& board_manager, int depth) {
    LOG_TRACE;

    calls = 0;
    BoardState& bs = board_manager.get_bs();
    tracked_array<Move, MAX_NUM_MOVES> moves = get_moves(bs.turn, bs);
    tracked_array<Move, MAX_NUM_MOVES> valid_moves = board_manager.get_legal_moves(moves);

    Move best = valid_moves[0];
    int alpha = INT_MIN;
    int beta = INT_MAX;

    for(Move move : valid_moves) {
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

    switch (BoardManager::get_game_state(valid_moves, in_check)) {
        case CHECKMATE: return bs.turn == ai_colour ? -(MATE_VALUE + depth) : (MATE_VALUE + depth);
        case STALEMATE: return 0;
        default: break;
    }

    int best_score = is_maximising ? INT_MIN : INT_MAX;
    for (Move move : valid_moves) {
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
int AIManager::eval_board(BoardState& bs) {
    int score = 0;

    // count pieces
    auto bbs = bs.piece_bbs();
    for (int i = 0; i < NUM_PIECE_BBS; i++) {
        Bitboard bb = bbs[i];

        PieceType bb_type = static_cast<PieceType>(i % (NUM_PIECE_BBS / 2));
        int score_multiplier = i < NUM_PIECE_BBS / 2 ? 1 : -1;

        int score_per_piece = PIECE_MAP[bb_type] * score_multiplier;
        while (bb) {
            pop_lsb(bb);
            score += score_per_piece;
        }
    }

    // pawns
    Bitboard& white_pawns = bs.get_bb(Colour::WHITE, PieceType::PAWN);
    Bitboard& black_pawns = bs.get_bb(Colour::BLACK, PieceType::PAWN);
    score += (COUNT_BITS(white_pawns & CENTRE) - COUNT_BITS(black_pawns & CENTRE)) * CENTRE_PAWN_VAL;
    score += (COUNT_BITS(white_pawns & EXTENDED_CENTRE) - COUNT_BITS(black_pawns & EXTENDED_CENTRE)) * EXTENDED_CENTRE_PAWN_VAL;

    // king safety
    Bitboard& white_king = bs.get_bb(Colour::WHITE, PieceType::KING);
    Bitboard& black_king = bs.get_bb(Colour::BLACK, PieceType::KING);
    score += (COUNT_BITS(white_king & KING_SAFETY) - COUNT_BITS(black_king & KING_SAFETY)) * KING_SAFETY_VAL;

    return ai_colour == Colour::WHITE ? score : -score; // flip so score is from AI's perspective
}