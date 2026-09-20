#include <doctest/doctest.h>

#include "perft.hpp"
#include "test_util.hpp"
#include "delegates/board_manager.hpp"
#include "utility/start_up.hpp"

// https://chessprogramming.org/Perft

TEST_SUITE("PERFT Tests") {
    /**
     * Performs the PERFT tests from the given starting FEN position.
     *
     * @param fen start position encoded as a FEN string
     * @param expected the list of correct numbers of nodes in the PERF-tree at various depths
     * @param depth the depth at which to perform this PERFT test
     */
    void perft_test(const std::string& fen, const std::vector<uint64_t> &expected, int depth) {
        REQUIRE_GE(depth, 1);
        REQUIRE_LE(depth, expected.size());

        SUBCASE(std::to_string(depth)) {
            init_static_data();
            BoardManager bm = BoardManager(fen);
            uint64_t nodes = perft(bm, depth);
            CHECK_EQ(nodes, expected[depth - 1]);
        }
    }

    /**
     * The below test case can be used to test the possible moves from a given PERFT subtree by:
     * init the board manager with the given FEN, or init with the starting chess FEN and then applying the
     * necessary moves in order, then calling divide.
     */
    TEST_CASE("Util Check") {
        // init_static_data();
        // BoardManager bm = BoardManager(START_FEN);
        // divide(bm, 6);
    }

    //
    // Note: for the below, you can change the GENERATE statements depending on how long you wish to wait for the tests
    //      to run.
    //

    TEST_CASE("Position 1") {
        static const std::vector<uint64_t> expected = {20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956, 2439530234167};
        int depth = GENERATE(1,2,3,4,5);
        perft_test(POSITION_ONE, expected, depth);
    }

    TEST_CASE("Position 2") {
        static const std::vector<uint64_t> expected = {48, 2039, 97862, 4085603, 193690690, 8031647685};
        int depth = GENERATE(1,2,3,4,5);
        perft_test(POSITION_TWO, expected, depth);
    }

    TEST_CASE("Position 3") {
        static const std::vector<uint64_t> expected = {14, 191, 2812, 43238, 674624, 11030083};
        int depth = GENERATE(1,2,3,4,5);
        perft_test(POSITION_THREE, expected, depth);
    }

    TEST_CASE("Position 4") {
        static const std::vector<uint64_t> expected = {6, 264, 9467, 422333, 15833292, 706045033};
        int depth = GENERATE(1,2,3,4,5);
        perft_test(POSITION_FOUR, expected, depth);
    }

    TEST_CASE("Position 5") {
        static const std::vector<uint64_t> expected = {44, 1486, 62379, 2103487, 89941194};
        int depth = GENERATE(1,2,3,4,5);
        perft_test(POSITION_FIVE, expected, depth);
    }

    TEST_CASE("Position 6") {
        static const std::vector<uint64_t> expected = {46, 2079, 89890, 3894594, 164075551, 6923051137, 287188994746, 11923589843526, 490154852788714};
        int depth = GENERATE(1,2,3,4,5);
        perft_test(POSITION_SIX, expected, depth);
    }
}
