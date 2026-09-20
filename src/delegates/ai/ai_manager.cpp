#include "ai_manager.hpp"

#include <cstdlib>
#include <chrono>

#include "mvvlva.hpp"
#include "../../utility/logger.hpp"

constexpr int INF = 30'000; // just an obsurdly large score value for a chess position which does not result in underflow/overflow
constexpr int MATE_VALUE = INT_MAX - 1000; // not so close to INT_MAX to prevent overflow
constexpr Bitboard CENTRE(0x1818000000);
constexpr Bitboard EXTENDED_CENTRE(0x3c3c3c3c0000);
constexpr Bitboard KING_SAFETY(0xe7000000000000e7);

constexpr int CENTRE_PAWN_VAL = 25;
constexpr int EXTENDED_CENTRE_PAWN_VAL = 15;
constexpr int KING_SAFETY_VAL = 30;

constexpr int TT_SIZE_MB = 4;

AIManager::AIManager(Colour ai_colour, BoardManager& board_manager)
    : board_manager(board_manager), ai_colour(ai_colour), history_heuristic{} {}

/**
 * Fetches the AI's determination of the best move given the current board position
 *
 * @param max_duration how long the AI is permitted to search for in milliseconds
 * @return the best `Move`
 */
std::pair<Move, int> AIManager::get_move_until_time(const int max_duration_ms) {
    LOG_TRACE;
    const auto start_time = std::chrono::steady_clock::now();
    const auto max_duration = std::chrono::milliseconds(max_duration_ms);
    BoardState& bs = board_manager.get_bs();

    // decay history heuristic such that between each AI move it has SOME idea of
    // good moves from before, but it doesn't overpower the model decision-making
    for (auto& row : history_heuristic) for (auto& v : row) v /= 2;

    // get the legal moves from the position
    tracked_array<Move, MAX_NUM_MOVES> moves = get_moves(bs.turn, bs);
    moves = board_manager.get_legal_moves(moves);
    ASSERT(moves.size() > 0, "Empty Valid Moves called on AImanager::get_move at root");
    Move best = moves[0];

    // iterative deepening := loop over all depths
    int depth = 1;
    auto duration = std::chrono::milliseconds(0);
    while (duration < max_duration && depth < AI_MAX_DEPTH) {

        // fetch the best move found so far for the given position up to the current depth (or empty move)
        // and sort the moves
        TTEntry* tt_entry = tt.at(bs.hash);
        Move tt_best_move = tt_entry ? tt_entry->best_move : Move();
        order_moves(moves, tt_best_move, depth, bs);

        // iterate over the moves
        int alpha = -INF;
        for(const Move& move : moves) {

            // make the move, search the tree, and undo the move
            board_manager.make_move(move);
            int score = -negamax(depth - 1, -INF, -alpha);
            board_manager.undo_move();

            // if the move causes an alpha-cutoff -> record it
            if (score > alpha) {
                alpha = score;
                best = move;
            }

            duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time);
            if (duration > max_duration) break;
        }

         depth++;
    }

    return {best, depth - 1};
}

/**
 * Performs a negamax search for the best move
 *
 * @return the score of the position
 */
int AIManager::negamax(
    int depth,
    int alpha,
    int beta
) {
    LOG_TRACE;
    BoardState& bs = board_manager.get_bs();

    if (board_manager.is_fifty_move_rule() || board_manager.is_three_fold_repetition() || board_manager.is_insufficient_material()) {
        return 0;
    }

    // tt logic to review already seen positions: if we already know the exact score for this position
    // -> return it, if we have a bounded score entry, then reduce the alpha-beta window as required
    TTEntry* entry = tt.at(bs.hash);
    Move tt_best_move;
    if (entry) {
        tt_best_move = entry->best_move;
        if (entry->depth >= depth) {
            switch (entry->flag) {
                case TTFlag::EXACT: return entry->score;
                case TTFlag::LOWERBOUND: alpha = std::max(alpha, entry->score); break;
                case TTFlag::UPPERBOUND: beta  = std::min(beta,  entry->score); break;
            }

            // if the window has closed -> return the score
            if (alpha >= beta) return entry->score;
        }
    }

    // if we've reached the end depth -> evaluate the board and return the score
    if(depth == 0) return (bs.turn == ai_colour ? 1 : -1) * eval_board(bs); // exit

    // generate moves
    bool in_check = board_manager.is_in_check(bs.turn);
    tracked_array<Move, MAX_NUM_MOVES> moves = get_moves(bs.turn, bs);
    moves = board_manager.get_legal_moves(moves, in_check);

    // other end-game rules already checked, now check for checkmate / stalemate
    if (moves.empty()) {
        return in_check ? -(MATE_VALUE + depth) : 0;
    }

    // order moves
    order_moves(moves, tt_best_move, depth, bs);

    const int original_alpha = alpha;
    const int original_beta = beta;
    int best_score = -INF;
    Move best_move;

    // iterate over moves
    for (const Move& move : moves) {

        // process move and explore subsequent states
        board_manager.make_move(move);
        int score = -negamax(depth - 1, -beta, -alpha);
        board_manager.undo_move();

        // update the best move if one is found
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
        alpha = std::max(alpha, score);

        // if an alpha-cutoff occurs (i.e. alpha-beta window has closed so this move would never be allowed
        // to be played under optimal play by the opposite side)
        if (alpha >= beta) {
            // when 'quiet move' (non-capture) causes a beta-cutoff -> record it for move ordering
            if (!is_capture(move, bs)) {
                history_heuristic[move.from][move.to] += depth * depth;
                if (move != killers[depth][0]) {
                    killers[depth][1] = killers[depth][0];
                    killers[depth][0] = move;
                }
            }
            break;
        }
    }

    TTFlag tt_flag = (best_score <= original_alpha) ? TTFlag::UPPERBOUND
                   : (best_score >= original_beta)  ? TTFlag::LOWERBOUND
                   : TTFlag::EXACT;

    tt.store(bs.hash, best_score, depth, tt_flag, best_move);
    return best_score;
}

/**
 * Provides a score for the given position. Does not consider the game state (e.g. check, draw, checkmate)
 * only consider the pieces on the board and their relative positions
 *
 * @return score for the position
 */
int AIManager::eval_board(const BoardState& bs) const {
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

void AIManager::order_moves(tracked_array<Move, MAX_NUM_MOVES>& moves, const Move& tt_best_move, int depth, const BoardState& bs) const {
    // compute a score per move
    std::array<int, MAX_NUM_MOVES> scores{};
    for (size_t i = 0; i < moves.size(); i++) {
        const Move& m = moves[i];
        if (m == tt_best_move) {
            scores[i] = 1'000'000;
            continue;

        }
        if (is_capture(m, bs)) {
            scores[i] = 100'000 + get_mvvlva_score(m, bs);
            continue;
        }

        if (m == killers[depth][0]) {
            scores[i] = 90'000;
            continue;
        }

        if (m == killers[depth][1]) {
            scores[i] = 80'000;
            continue;
        }

        scores[i] = history_heuristic[m.from][m.to];
    }

    // sort moves by score, descending (simple insertion sort is fine for ~30-40 moves)
    for (size_t i = 1; i < moves.size(); i++) {
        Move mv = moves[i]; int sc = scores[i];
        int j = i - 1;
        while (j >= 0 && scores[j] < sc) {
            moves[j+1] = moves[j]; scores[j+1] = scores[j]; j--;
        }
        moves[j+1] = mv; scores[j+1] = sc;
    }
}
