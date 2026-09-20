#pragma once
#include "delegates/move_generator.hpp"
#include "utility/zobrist_hashing.hpp"

#include "utility/util.hpp"

constexpr const char* POSITION_ONE = START_FEN;
constexpr const char* POSITION_TWO = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
constexpr const char* POSITION_THREE = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ";
constexpr const char* POSITION_FOUR = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1 ";
constexpr const char* POSITION_FIVE = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ";
constexpr const char* POSITION_SIX = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ";