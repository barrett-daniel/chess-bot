#include <doctest/doctest.h>

#include "delegates/board_manager.hpp"

// https://chessprogramming.org/Perft

std::string sq_to_str(int sq) {
    char file = 'a' + (sq % 8);
    char rank = '0' + (8 - (sq / 8));
    return std::string(1, file) + std::string(1, rank);
}

std::string move_to_str(const Move& m) {
    std::string s = sq_to_str(m.from) + sq_to_str(m.to);
    if (m.promotion_piece != NONE) {
        switch (m.promotion_piece) {
            case QUEEN:  s += 'q'; break;
            case ROOK:   s += 'r'; break;
            case BISHOP: s += 'b'; break;
            case KNIGHT: s += 'n'; break;
            default: break;
        }
    }
    return s;
}

TEST_SUITE("PERFT") {
    uint64_t perft(BoardManager& bm, int depth) {
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

    /**
     * Performs PERFT up to the given depth, counting the number of resulting nodes in the PERFT-trees
     * generated from each possible move at the current state. Logs a message for the number of nodes
     */
    void divide(BoardManager& bm, int depth) {
        std::vector<Move> moves = get_moves(bm.get_bs().turn, bm.get_bs());
        std::vector<Move> legal_moves = bm.get_legal_moves(moves);

        uint64_t total = 0;
        for (const Move& move : legal_moves) {
            bm.make_move(move);
            uint64_t num = (depth == 1) ? 1 : perft(bm, depth - 1);
            bm.undo_move();

            MESSAGE(move_to_str(move) << ": " << num);
            total += num;
        }
        MESSAGE("Total: " << total);
    }

    /**
     * The below test case can be used to test the possible moves from a given PERFT subtree by:
     * init the board manager with the given FEN, or init with the starting chess FEN and then applying the
     * necessary moves in order, then calling divide.
     */
    TEST_CASE("Util Check") {
        BoardManager bm = BoardManager(START_FEN);
        divide(bm, 4);
    }

    /**
     * Performs PERFT as defined here: https://chessprogramming.org/Perft
     *
     * Checks against the known number of possible chess positions from the starting position up to a given depth.
     * Depending on the length at which you wish to run the tests, you can alter the GENERATE statement to search a
     * greater depth.
     */
    TEST_CASE("Possible Move Count") {
        uint64_t expected[] = {20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956, 2439530234167, 69352859712417};

        int d = GENERATE(1, 2, 3, 4, 5);

        SUBCASE(std::to_string(d)) {
            BoardManager bm = BoardManager(START_FEN);
            uint64_t nodes = perft(bm, d);
            CHECK_EQ(nodes, expected[d - 1]);
        }
    }
}
