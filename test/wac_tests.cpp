#include <format>
#include <fstream>
#include <doctest/doctest.h>

#include "test_util.hpp"
#include "delegates/ai/ai_manager.hpp"
#include "delegates/board_manager.hpp"
#include "utility/start_up.hpp"

/**
 * Based upon the "Win At Chess" book by Fred Reinfeld (1958), this test suite runs 300 chess positions
 * with deterministic 'best moves'. The suite verifies the AI can find these best moves.
 *
 * Generally this suite is good for sanity checking the AI still works, not arguing its elo or determining
 * if the model has gotten any better.
 */

TEST_SUITE("Win At Chess") {
    constexpr const char* DATABASE_FPATH = "../../test/resources/wac_database.txt";
    constexpr int NUM_CASES = 300;
    constexpr int AI_TEST_DEPTH = 5;

    // ------------------------------------------------------------------------
    // Chess Notation Move Parsing
    // ------------------------------------------------------------------------

    PieceType to_piece_type(char c) {
        c = std::tolower(c);
        switch (c) {
            case 'q': return PieceType::QUEEN;
            case 'k': return PieceType::KING;
            case 'r': return PieceType::ROOK;
            case 'b': return PieceType::BISHOP;
            case 'n': return PieceType::KNIGHT;
            case 'p': return PieceType::PAWN;
            default: return PieceType::NONE;
        }
    }

    /**
     * Converts the string representing a square on the board to its index position
     */
    inline Square from_string_sq(const std::string& s) {
        if (s.size() != 2) throw std::invalid_argument("Bad square string: " + s);

        int file = s[0] - 'a';
        int rank = s[1] - '0';

        if (file < 0 || file > 7 || rank < 1 || rank > 8) throw std::invalid_argument("Bad square string: " + s);

        return (rank - 1) * 8 + file;
    }

    /**
     * Parses an algebraic chess notation move string into a move struct
     */
    Move to_move(const std::string& input, const tracked_array<Move, MAX_NUM_MOVES>& legal_moves) {
        std::string san = input;

        // Strip check suffixes (+)
        if (san.back() == '+') san.pop_back();
        if (san.size() < 2) throw std::invalid_argument("Invalid SAN move: " + input);

        // Destination square is always the last two characters at this point
        std::string dest_str = san.substr(san.size() - 2);
        Square to = from_string_sq(dest_str);
        san.erase(san.size() - 2);

        // remove capture suffix (x)
        if (san.back() == 'x') san.pop_back();

        // Moving piece: letter for piece, or PAWN if absent
        PieceType piece = PieceType::PAWN;
        PieceType first_char_type = to_piece_type(san[0]);
        if (first_char_type != PieceType::NONE) {
            piece = first_char_type;
            san.erase(0, 1);
        }

        // Whatever's left (0-2 chars) is a disambiguating file and/or rank,
        // e.g. "Nbd2" -> "b", "N1d2" -> "1", "Qh4e1" -> "h4"
        int disambig_file = -1, disambig_rank = -1;
        for (char c : san) {
            if (c >= 'a' && c <= 'h') disambig_file = c - 'a';
            else if (c >= '1' && c <= '8') disambig_rank = c - '0';
        }


        // find matches
        tracked_array<Move, MAX_NUM_MOVES> matches;
        for (const auto& m : legal_moves) {
            if (m.moved_piece != piece) continue;
            if (m.to != to) continue;

            if (disambig_file != -1 || disambig_rank != -1) {
                int from_file = m.from % 8;
                int from_rank = (m.from / 8) + 1;
                if (disambig_file != -1 && from_file != disambig_file) continue;
                if (disambig_rank != -1 && from_rank != disambig_rank) continue;
            }


            matches.push_back(m);
        }

        if (matches.empty()) throw std::invalid_argument("No legal move matches SAN: " + input);
        if (matches.size() > 1) throw std::invalid_argument("Ambiguous SAN move, needs more disambiguation: " + input);

        return matches[0];
    }

    /**
     * Converts a string of possible multiple moves in algebraic chess notation to the Move struct
     *
     * @param moves_str string of moves in algebraic notation
     * @param bm the board manager for which these moves are possible within the current state
     * @return
     */
    tracked_array<Move, MAX_NUM_MOVES> to_moves(const std::string& moves_str, BoardManager& bm) {
        tracked_array<Move, MAX_NUM_MOVES> moves;

        std::vector<std::string> chunks;
        std::stringstream ss(moves_str);

        std::string elem;
        while (std::getline(ss, elem, ' ')) {
            chunks.push_back(elem);
        }

        tracked_array<Move, MAX_NUM_MOVES> legal_moves = bm.get_legal_moves(get_moves(bm.get_bs().turn, bm.get_bs()));
        for (const std::string& chunk : chunks) {
            moves.push_back(to_move(chunk, legal_moves));
        }

        return moves;
    }

    // ------------------------------------------------------------------------
    // EPD Parsing
    // ------------------------------------------------------------------------

    static const std::vector<std::string> OPCODES = {"bm", "am", "c0", "id"};
    enum OpcodeType {
        FEN,
        BEST_MOVE,
        AVOID_MOVE,
        COMMENT,
        ID
    };

    /**
     * Convert an EPD opcode string to its enum type
     */
    OpcodeType to_opcode_type(const std::string& op) {
        if (op == "bm") return BEST_MOVE;
        if (op == "am") return AVOID_MOVE;
        if (op == "c0") return COMMENT;
        if (op == "id") return ID;

        throw std::invalid_argument("invalid opcode type: " + op);
    }

    struct TestCaseData {
        BoardManager bm;
        tracked_array<Move, MAX_NUM_MOVES> best_moves;
        tracked_array<Move, MAX_NUM_MOVES> avoid_moves;

    };

    /**
     * Parses a line of Extended Position Description into a suitable test data struct.
     */
    TestCaseData parse_epd_line(const std::string& line) {
        TestCaseData data;
        std::vector<std::string> chunks;

        std::string elem;
        std::stringstream ss(line);
        while (std::getline(ss, elem, ';')) {
            // trim leading space left over from the split
            if (elem.front() == ' ') {
                elem = elem.substr(1);
            }

            chunks.push_back(elem);
        }

        bool bm_set = false;
        for (const std::string& chunk : chunks) {
            // find the str position of the opcode
            size_t pos = std::string::npos;
            for (const std::string& op_code : OPCODES) {
                pos = chunk.find(op_code);

                if (pos != std::string::npos) break;
            }

            // find the contents of the opcode and map the raw opcode to the enum type.
            // Note: FENs don't have a preceding opcode value and so are the default value.
            OpcodeType opcode = FEN;
            std::string contents = chunk;
            if (pos != std::string::npos) {
                std::string opcode_str = chunk.substr(pos, 2);
                opcode = to_opcode_type(opcode_str);
                contents = chunk.substr(pos + 3);
            }

            // encode the contents of the opcode chunk into the struct
            switch (opcode) {
                case FEN:
                    data.bm = BoardManager(contents);
                    bm_set = true;
                    break;

                case BEST_MOVE:
                    ASSERT(bm_set, "FEN was not passed before best_move");
                    data.best_moves = to_moves(contents, data.bm);
                    break;

                case AVOID_MOVE:
                    ASSERT(bm_set, "FEN was not passed before avoid_move");
                    data.avoid_moves = to_moves(contents, data.bm);
                    break;

                default: break;
            }
        }

        return data;
    }

    // ------------------------------------------------------------------------
    // TESTS
    // ------------------------------------------------------------------------

    /**
     * @param index of line
     * @return line at given index in database
     */
    std::string read_line(int index) {
        std::ifstream ifs(DATABASE_FPATH);
        std::string line;
        for (int i = 0; i <= index; ++i) {
            if (!std::getline(ifs, line)) throw std::runtime_error("Not enough lines in test database");
        }
        return line;
    }

    /**
     * Runs the AI against the database, ensuring it finds the (one of) the best possible moves
     * and avoids the worst moves (if provided)
     *
     * @param index of the line in the database for the test case
     */
    void run_ai_test_case(int index) {
        TestCaseData tc = parse_epd_line(read_line(index));

        AIManager ai(tc.bm.get_bs().turn, tc.bm, AI_SEARCH_TIME_MS);
        SearchData search_data;
        Move ai_move = ai.get_move(&search_data);

        bool found_best = false;
        for (const Move& move : tc.best_moves)
            if (move == ai_move) { found_best = true; break; }

        bool avoided_worst = true;
        for (const Move& move : tc.avoid_moves)
            if (move == ai_move) { avoided_worst = false; break; }


        std::string best_moves_str;
        for (const Move& move : tc.best_moves) best_moves_str += move.to_string() + " ";

        std::string avoid_moves_str;
        for (const Move& move : tc.avoid_moves) avoid_moves_str += move.to_string() + " ";

        std::string log_msg = std::format(
            "\nAi move chosen: {}\nBest moves: {}\nAvoid moves: {}\nAI searched to a depth: {}\n",
            ai_move.to_string(),
            best_moves_str,
            avoid_moves_str,
            search_data.depth
        );
        MESSAGE(log_msg);
        CHECK(found_best);
        CHECK(avoided_worst);
    }

    /**
     * Runs the entire database
     */
    TEST_CASE("Win At Chess") {
        init_static_data();

        for (int i = 0; i < NUM_CASES; ++i) {
            SUBCASE(("case " + std::to_string(i)).c_str()) {
                run_ai_test_case(i);
            }
        }

    }

    /**
     * Util test case: update the index to run a specific test case
     */
    TEST_CASE("Run Specific") {
        constexpr int INDEX = 1;

        init_static_data();
        run_ai_test_case(INDEX);
    }
}