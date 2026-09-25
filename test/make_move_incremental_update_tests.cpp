
#include <random>
#include <doctest/doctest.h>

#include "test_util.hpp"
#include "board_data/board_manager.hpp"
#include "ai/eval_board.hpp"
#include "util/init_static_data.hpp"

constexpr int TEST_NUM_ITERATIONS = 30;
constexpr int TEST_NUM_MOVES = 30;

struct PlayedMove {
    std::string fen_before;
    std::string move_str;
};

void dump_failure(
    const std::string& start_fen,
    const tracked_array<PlayedMove, TEST_NUM_MOVES>& history,
    const BoardState& bs,
    const std::string& err_msg,
    auto incremental_value,
    auto static_value
) {
    std::cerr << "\n=== TEST FAILURE: " << err_msg << " ===\n";
    std::cerr << "Starting FEN: " << start_fen << "\n";
    std::cerr << "Incremental value: " << incremental_value << "\n";
    std::cerr << "Statically computed value:  " << static_value << "\n";
    std::cerr << "Move history (" << history.size() << " moves):\n";
    for (size_t i = 0; i < history.size(); i++) {
        std::cerr << "  [" << i << "] " << history[i].move_str;
    }
    std::cerr << "FEN immediately before failing move: " << (history.empty() ? start_fen : history.back().fen_before) << "\n";
    std::cerr << "FEN immediately after failing move: " << bs.to_fen() << "\n";
};

void run_test_case(const std::string& fen, bool testing_eval, bool testing_hash) {
    for (int i = 0; i < TEST_NUM_ITERATIONS; i++) {
        init_static_data();
        BoardManager bm = BoardManager(fen);
        BoardState& bs = bm.get_bs();
        tracked_array<PlayedMove, TEST_NUM_MOVES> history;

        for (int j = 0; j < TEST_NUM_MOVES; j++) {
            auto moves = get_moves(bs.turn, bs);
            moves = bm.get_legal_moves(moves);

            if (moves.empty()) break;  // checkmate/stalemate — avoid % 0
            Move chosen = moves[rand_u64() % moves.size()];

            std::string fen_before = bs.to_fen();
            std::string move_str = chosen.to_string();
            bm.make_move(chosen);
            history.emplace_back(std::move(fen_before), std::move(move_str));

            if (testing_eval) {
                auto static_eval = eval_board(bs);
                if (bs.edata != static_eval) {
                    dump_failure(fen, history, bs, "Incremental eval (edata) mismatch", bs.edata, static_eval);
                    FAIL("");
                    return;
                }
            }

            if (testing_hash) {
                auto static_hash = zhash(bs);
                if (bs.hash != static_hash) {
                    dump_failure(fen, history, bs, "Zobrist hash mismatch", bs.hash, static_hash);
                    FAIL("");
                    return;
                }
            }

            if (bm.is_game_over()) break;
        }
    }
}

TEST_SUITE("Incremental Hash Tests") {
    TEST_CASE("Position 1") {
        run_test_case(POSITION_ONE, false, true);
    }

    TEST_CASE("Position 2") {
        run_test_case(POSITION_TWO, false, true);
    }

    TEST_CASE("Position 3") {
        run_test_case(POSITION_THREE, false, true);
    }

    TEST_CASE("Position 4") {
        run_test_case(POSITION_FOUR, false, true);
    }

    TEST_CASE("Position 5") {
        run_test_case(POSITION_FIVE, false, true);
    }

    TEST_CASE("Position 6") {
        run_test_case(POSITION_SIX, false, true);
    }
}

TEST_SUITE("Incremental Board Evaluation Tests") {
    TEST_CASE("Position 1") {
        run_test_case(POSITION_ONE, true, false);
    }

    TEST_CASE("Position 2") {
        run_test_case(POSITION_TWO, true, false);
    }

    TEST_CASE("Position 3") {
        run_test_case(POSITION_THREE, true, false);
    }

    TEST_CASE("Position 4") {
        run_test_case(POSITION_FOUR, true, false);
    }

    TEST_CASE("Position 5") {
        run_test_case(POSITION_FIVE, true, false);
    }

    TEST_CASE("Position 6") {
        run_test_case(POSITION_SIX, true, false);
    }
}