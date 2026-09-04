#include <doctest/doctest.h>

#include "delegates/board_manager.hpp"

// https://chessprogramming.org/Perft

TEST_SUITE("PERFT") {
    static uint64_t perft(BoardManager& bm, int depth) {
        std::vector<Move> moves = get_moves(bm.get_bs().turn, bm.get_bs());
        std::vector<Move> legal_moves = bm.get_legal_moves(moves);

        if (depth == 1) {
            return legal_moves.size();
        }

        uint64_t nodes = 0;
        for (const Move& move : legal_moves) {
            bm.make_move(move);
            nodes += perft(bm, depth - 1);
            bm.undo_move();
        }
        return nodes;
    }

    TEST_CASE("Possible Move Count") {
        uint64_t expected[] = {20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956, 2439530234167, 69352859712417};
        const int max_depth = 6;

        int d = GENERATE(1, 2, 3, 4, 5, 6);

        SUBCASE(std::to_string(d)) {
            BoardManager bm = BoardManager(START_FEN);
            uint64_t nodes = perft(bm, d);
            CHECK_EQ(nodes, expected[d - 1]);
        }
    }
}
