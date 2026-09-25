#include "ai_manager.hpp"

#include <cstdlib>
#include <chrono>

#include "mvvlva.hpp"
#include "../../utility/logger.hpp"
#include "eval_board.hpp"

constexpr int INF = 1'000'000;
constexpr int MATE_VALUE = INF - 1'000;
constexpr int TT_SIZE_MB = 4;

AIManager::AIManager(Colour ai_colour, BoardManager& board_manager, int search_limit_ms)
    : board_manager(board_manager), ai_colour(ai_colour), history_heuristic{}, search_limit(search_limit_ms) {}

/**
 * Fetches the AI's determination of the best move given the current board position
 */
Move AIManager::get_move(SearchData* search_data) {
    LOG_TRACE;
    BoardState& bs = board_manager.get_bs();

    // init time control vars
    search_start = std::chrono::steady_clock::now();
    search_aborted = false;

    // decay history heuristic such that between each AI move it has SOME idea of
    // good moves from before, but it doesn't overpower the model decision-making
    for (auto& row : history_heuristic) for (auto& v : row) v /= 2;

    // get the legal moves from the position
    tracked_array<Move, MAX_NUM_MOVES> moves = get_moves(bs.turn, bs);
    moves = board_manager.get_legal_moves(moves);
    ASSERT(!moves.empty(), "Empty Valid Moves called on AImanager::get_move at root");
    Move best = moves[0];
    int completed_depth = 0;

    // iterative deepening := loop over all depths
    uint64_t num_visited_nodes = 0;
    int depth = 1;
    while (depth < AI_MAX_DEPTH) {

        // fetch the best move found so far for the given position up to the current depth (or empty move)
        // and sort the moves
        TTEntry* tt_entry = tt.at(bs.hash);
        Move tt_best_move = tt_entry ? tt_entry->best_move : Move();
        order_moves(moves, tt_best_move, depth, bs);

        // iterate over the moves
        int alpha = -INF;
        Move iteration_best = moves[0];
        bool iteration_complete = true;

        for(const Move& move : moves) {

            // make the move, search the tree, and undo the move
            board_manager.make_move(move);
            int score = -negamax(depth - 1, -INF, -alpha, num_visited_nodes);
            board_manager.undo_move();

            if (search_aborted) {
                iteration_complete = false;
                break;
            }

            // if the move causes an alpha-cutoff -> record it
            if (score > alpha) {
                alpha = score;
                iteration_best = move;
            }

            if (std::chrono::steady_clock::now() - search_start > search_limit) {
                iteration_complete = false;
                break;
            }
        }

        if (iteration_complete) {
            best = iteration_best;
            completed_depth = depth;
        }

        if (search_aborted || !iteration_complete) break;

        depth++;
    }

    if (search_data) {
        search_data->depth = completed_depth;
        search_data->nodes = num_visited_nodes;
        search_data->elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - search_start);
    }
    return best;
}

/**
 * Performs a negamax search for the best move
 *
 * @return the score of the position
 */
int AIManager::negamax(
    int depth,
    int alpha,
    int beta,
    uint64_t& num_visited_nodes
) {
    LOG_TRACE;

    if ((++num_visited_nodes & 0x7FF) == 0) { // check every 2048 nodes
        if (std::chrono::steady_clock::now() - search_start > search_limit) {
            search_aborted = true;
        }
    }
    if (search_aborted) return 0; // value ignored by caller, just unwind fast

    if (board_manager.is_fifty_move_rule() || board_manager.is_three_fold_repetition() || board_manager.is_insufficient_material()) {
        return 0;
    }

    BoardState& bs = board_manager.get_bs();

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
    if(depth == 0) return (bs.turn == Colour::WHITE ? 1 : -1) * bs.edata.get_score(); // exit

    // generate moves
    bool in_check = board_manager.is_in_check(bs.turn);
    tracked_array<Move, MAX_NUM_MOVES> moves = get_moves(bs.turn, bs);
    moves = board_manager.get_legal_moves(moves, in_check);

    // other end-game rules already checked, now check for checkmate / stalemate
    if (moves.empty()) {
        // std::cout << bs.to_string() << std::endl;
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
        int score = -negamax(depth - 1, -beta, -alpha, num_visited_nodes);
        board_manager.undo_move();

        if (search_aborted) return 0;   // bail out with no TT write, no score usage

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
