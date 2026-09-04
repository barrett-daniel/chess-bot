#include <doctest/doctest.h>

#include "../src/pieces/pawn.hpp"
#include "../src/utility/util.hpp"

#define EMPTY 0x0

// 8  . . . . . . . . 
// 7  . . . . . . . . 
// 6  . . . . . . . . 
// 5  . . . . . . . . 
// 4  . . . . . . . . 
// 3  . . . . . . . . 
// 2  . . . . . . . . 
// 1  . . . . . . . . 
//    A B C D E F G H

TEST_SUITE("Single_pushes") {
    std::vector<Move> generate_moves(int init_pos, bool is_white) {
        Bitboard pawn_bb;
        pawn_bb.set(init_pos);

        Bitboard enemy_bb(EMPTY);
        Bitboard all_bb = pawn_bb | enemy_bb;
        int en_passant_target_sq = -1;

        return generate_pawn_moves(
            pawn_bb,
            is_white,
            enemy_bb,
            all_bb,
            en_passant_target_sq
        );
    }

    TEST_CASE("Normal") {
        std::vector<int> inital_positions = {A3, D3, H3, A6, D6, H6};

        auto do_common_checks = [](std::vector<Move> moves, int init_pos) {
            Move move = moves.front();

            CHECK_EQ(1, moves.size());
            CHECK_EQ(init_pos, move.from);
            CHECK_EQ(PieceType::PAWN, move.moved_piece);
            CHECK_EQ(PieceType::NONE, move.promotion_piece);
            CHECK(!move.is_en_passant);
            CHECK(!move.is_castling);
        };

        SUBCASE("WHITE") {
            for(int i : inital_positions) {
                CAPTURE(i);

                std::vector<Move> moves = generate_moves(i, true);
                do_common_checks(moves, i);
                CHECK_EQ(i - BOARD_SIZE, moves.front().to);
            }
        }

        SUBCASE("BLACK") {
            for(int i : inital_positions) {
                CAPTURE(i);

                std::vector<Move> moves = generate_moves(i, false);
                do_common_checks(moves, i);
                CHECK_EQ(i + BOARD_SIZE, moves.front().to);
            }
        }
    }
}

/*
1. Check single pushes work
2. Check captures works
3. Check enpassant works
4. Check double pushes work
5. Check promotion pushes work and their types
6. Check blocked single pushes
*/