#include <chrono>
#include <doctest/doctest.h>
#include <fstream>
#include <numeric>

#include "test_util.hpp"
#include "ai/ai_manager.hpp"
#include "util/init_static_data.hpp"

/**
 *  There is no pass/fail for this test case. Its purpose is to write to the speed_test_results.csv
 *  to benchmark model performance over time. The model is ran against the test positions with the AI
 *  making moves for both black and white.
 *
 *  Tracks search depth, node count, and elapsed time per move to compute average depth and
 *  nodes-per-second (NPS) per position, which are more meaningful speed indicators than wall-clock
 *  time alone, since get_move_until_time runs on a fixed time budget.
 *
 *  The 'Tag' column of the csv should be added manually AFTER running the test.
 *
 *  These tests should be ran in RELEASE mode.
 */
TEST_SUITE("Speed Tests") {
    constexpr const char* RESULTS_FPATH = "../../test/resources/speed_test_results.csv";

    // parameters for run configuration
    constexpr int NUM_FULL_MOVES = 5;
    constexpr int NUM_RUNS = 5;

    struct PositionStats {
        double avg_depth = 0;
        int min_depth = std::numeric_limits<int>::max();
        int max_depth = 0;
        u64 total_nodes = 0;
        double nodes_per_second = 0;
        long long average_microseconds_per_move = 0;
    };

    /**
     * @param fen start position of the test case
     * @return aggregated depth/node/timing stats for the bots playing against each other,
     *         based on the run configuration
     */
    PositionStats run_position(const std::string& fen) {
        init_static_data();

        u64 total_nodes = 0;
        std::chrono::microseconds total_time{0};
        long long depth_sum = 0;
        int min_depth = std::numeric_limits<int>::max();
        int max_depth = 0;
        int total_plies = 0;

        // run the test `n` times
        for (int i = 0; i < NUM_RUNS; i++) {
            BoardManager bm = BoardManager(fen);
            AIManager black_ai = AIManager(Colour::BLACK, bm, AI_SEARCH_TIME_MS);
            AIManager white_ai = AIManager(Colour::WHITE, bm, AI_SEARCH_TIME_MS);

            Colour turn = bm.get_bs().turn;
            AIManager* player_one = turn == Colour::WHITE ? &white_ai : &black_ai;
            AIManager* player_two = turn == Colour::WHITE ? &black_ai : &white_ai;

            for (int j = 0; j < NUM_FULL_MOVES; j++) {
                for (AIManager* player : {player_one, player_two}) {
                    SearchData search_data{};
                    Move move = player->get_move(&search_data);
                    bm.make_move(move);

                    total_nodes += search_data.nodes;
                    total_time += search_data.elapsed_time;
                    depth_sum += search_data.depth;
                    min_depth = std::min(min_depth, search_data.depth);
                    max_depth = std::max(max_depth, search_data.depth);
                    total_plies++;

                    if (bm.is_game_over()) goto next_run;
                }
            }
            next_run:;
        }

        auto safe_divide = [](auto lhs, auto rhs) {
            return rhs > 0 ? lhs / rhs : 0;
        };

        PositionStats stats;
        stats.avg_depth = safe_divide(static_cast<double>(depth_sum), total_plies);
        stats.min_depth = min_depth;
        stats.max_depth = max_depth;
        stats.total_nodes = total_nodes;
        stats.nodes_per_second = safe_divide(static_cast<double>(total_nodes), (total_time.count() / 1'000'000.0));
        stats.average_microseconds_per_move = safe_divide(total_time.count(), total_plies);
        return stats;
    }

    TEST_CASE("Speed Tests") {
        init_static_data();

        bool file_exists = std::ifstream(RESULTS_FPATH).good();
        std::ofstream results_file(RESULTS_FPATH, std::ios_base::app);
        results_file << std::fixed << std::setprecision(3); // to prevent e-notation for large numbers

        if (!file_exists) {
            results_file << "Tag,Position,AvgDepth,MinDepth,MaxDepth,TotalNodes,NPS,AvgUsPerMove"
                         << std::endl;
        }

        std::vector<std::pair<std::string, std::string>> positions = {
            {"POSITION_ONE", POSITION_ONE},
            {"POSITION_TWO", POSITION_TWO},
            {"POSITION_THREE", POSITION_THREE},
            {"POSITION_FOUR", POSITION_FOUR},
            {"POSITION_FIVE", POSITION_FIVE},
            {"POSITION_SIX", POSITION_SIX},
        };

        for (const auto& [name, fen] : positions) {
            PositionStats stats = run_position(fen);

            // leading comma leaves room to manually fill in the 'Tag' column, as before
            results_file << "," << name << ","
                         << stats.avg_depth << ","
                         << stats.min_depth << ","
                         << stats.max_depth << ","
                         << stats.total_nodes << ","
                         << stats.nodes_per_second << ","
                         << stats.average_microseconds_per_move
                         << std::endl;

            MESSAGE(name << " complete");
        }

        results_file.close();
    }
}