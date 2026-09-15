#include <doctest/doctest.h>

#include "test_util.hpp"
#include "delegates/board_manager.hpp"

TEST_SUITE("Draw State Tests") {
    void run_test_case(const std::string& fen, const std::vector<Move> moves) {
        before_each();

        BoardManager bm = BoardManager(fen);

        for (const Move& move : moves) {
            bm.make_move(move);
        }

        GameState state = bm.get_game_state();
        CHECK_EQ(state, GameState::DRAW);
    }

    TEST_CASE("50 Move Rule From FEN") {
        run_test_case("8/6k1/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 w - - 100 51", {});
    }

    TEST_CASE("50 Move Rule After Some Moves") {
        run_test_case("8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50", {
            Move(F7, G7, PieceType::KING)
        });
    }

    TEST_CASE("Minimal 3 Fold Repetition") {
        run_test_case(START_FEN, {
            Move(G1, F3, PieceType::KNIGHT),
            Move(G8, F6, PieceType::KNIGHT),
            Move(F3, G1, PieceType::KNIGHT),
            Move(F6, G8, PieceType::KNIGHT),

            Move(G1, F3, PieceType::KNIGHT),
            Move(G8, F6, PieceType::KNIGHT),
            Move(F3, G1, PieceType::KNIGHT),
            Move(F6, G8, PieceType::KNIGHT)
        });
    }

    TEST_CASE("Expanded 3 Fold Repetition") {
        run_test_case(START_FEN, {
            Move(G1, F3, PieceType::KNIGHT),
            Move(G8, F6, PieceType::KNIGHT),
            Move(B1, C3, PieceType::KNIGHT),
            Move(B8, C6, PieceType::KNIGHT),
            Move(F3, G1, PieceType::KNIGHT),
            Move(F6, G8, PieceType::KNIGHT),
            Move(C3, B1, PieceType::KNIGHT),
            Move(C6, B8, PieceType::KNIGHT),

            Move(G1, F3, PieceType::KNIGHT),
            Move(G8, F6, PieceType::KNIGHT),
            Move(B1, C3, PieceType::KNIGHT),
            Move(B8, C6, PieceType::KNIGHT),
            Move(F3, G1, PieceType::KNIGHT),
            Move(F6, G8, PieceType::KNIGHT),
            Move(C3, B1, PieceType::KNIGHT),
            Move(C6, B8, PieceType::KNIGHT)
        });
    }

    TEST_CASE("Stalemate") {
        run_test_case("k7/2Q5/8/8/8/8/8/K7 b - - 0 1", {});
    }

    TEST_CASE("Only Kings") {
        run_test_case("k7/8/8/8/8/8/8/K7 w - - 0 1", {});
    }
}
