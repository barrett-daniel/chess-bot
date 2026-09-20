// DEPRECATED
//
// #include <chrono>
// #include <doctest/doctest.h>
// #include <fstream>
//
// #include "test_util.hpp"
// #include "delegates/ai/ai_manager.hpp"
// #include "utility/start_up.hpp"
//
// /**
//  *  There is no pass/fail for this test case. Its purpose is to write to the speed_test_results.csv
//  *  to benchmark model performance over time. The model is ran against the test positions with the AI
//  *  making moves for both black and white.
//  *
//  *  The 'Tag' column of the csv should be added manually AFTER running the test.
//  *
//  *  These tests should be ran in RELEASE mode.
//  */
// TEST_SUITE("Speed Tests") {
//     constexpr const char* RESULTS_FPATH = "../../test/resources/speed_test_results.csv";
//
//     // parameters for run configuration
//     constexpr int TEST_DEPTH = 5;
//     constexpr int NUM_FULL_MOVES = 5;
//     constexpr int NUM_RUNS = 3;
//
//     /**
//      * @param fen start position of the test case
//      * @return an average of the time taken for the bots to play against each other based on the run configuration
//      */
//     std::chrono::microseconds run_position(const std::string& fen) {
//         init_static_data();
//
//         std::chrono::microseconds total_time{0};
//         int total_plies = 0;
//
//         // run the test `n` times
//         for (int i = 0; i < NUM_RUNS; i++) {
//             BoardManager bm = BoardManager(fen);
//             AIManager black_ai = AIManager(Colour::BLACK, bm);
//             AIManager white_ai = AIManager(Colour::WHITE, bm);
//
//             Colour turn = bm.get_bs().turn;
//             AIManager* player_one = turn == Colour::WHITE ? &white_ai : &black_ai;
//             AIManager* player_two = turn == Colour::WHITE ? &black_ai : &white_ai;
//
//             // time how long it takes to compute the test case
//             int plies = 0;
//             auto start = std::chrono::steady_clock::now();
//             for (int j = 0; j < NUM_FULL_MOVES; j++) {
//                 bm.make_move(player_one->get_move_until_time(AI_SEARCH_TIME_MS));
//                 plies++;
//                 if (is_game_over(bm.get_game_state())) break;
//
//                 bm.make_move(player_two->get_move_until_time(AI_SEARCH_TIME_MS));
//                 plies++;
//                 if (is_game_over(bm.get_game_state())) break;
//             }
//             auto stop = std::chrono::steady_clock::now();
//
//             total_time += std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
//             total_plies += plies;
//         }
//
//         // average the results
//         return total_time / total_plies;
//     }
//
//     TEST_CASE("Speed Tests") {
//         init_static_data();
//
//         std::ofstream results_file(RESULTS_FPATH, std::ios_base::app);
//         results_file << ",";
//
//         results_file << run_position(POSITION_ONE).count() << ", ";
//         MESSAGE("Position ONE complete");
//
//         results_file << run_position(POSITION_TWO).count() << ", ";
//         MESSAGE("Position TWO complete");
//
//         results_file << run_position(POSITION_THREE).count() << ", ";
//         MESSAGE("Position THREE complete");
//
//         results_file << run_position(POSITION_FOUR).count() << ", ";
//         MESSAGE("Position FOUR complete");
//
//         results_file << run_position(POSITION_FIVE).count() << ", ";
//         MESSAGE("Position FIVE complete");
//
//         results_file << run_position(POSITION_SIX).count();
//         MESSAGE("Position SIX complete");
//
//         results_file << std::endl;
//         results_file.close();
//     }
// }