#include "board_manager.hpp"

#include <sstream>
#include <utility>

#include "zobrist_hashing.hpp"
#include "../utility/logger.hpp"
#include "../pieces/pieces.hpp"

// ----------------------------------------------
// Constructor
// ----------------------------------------------

BoardManager::BoardManager(const std::string& fen) {
    LOG_TRACE;
    history.reserve(50); // allocate sufficiently large amount of space to prevent vector resizing

    // decompose FEN into component sections
    std::stringstream ss(fen);
    std::vector<std::string> components;
    std::string elem;
    while(ss >> elem) components.push_back(elem);
    ASSERT(components.size() <= 6, "Malformed FEN: too many arguments provided");
    ASSERT(components.size() >= 4, "Malformed FEN: too few arguments provided");

    // determine whose first to move
    std::string turn_config_str = components[FEN_TURN_CONFIG];
    bs.turn = turn_config_str == "w" ? Colour::WHITE : Colour::BLACK;

    // initalise board data
    std::string board_config_str = components[FEN_BOARD_CONFIG];
    int i = 0, j = 7;
    for(char c : board_config_str) {
        std::string c_str(1, c);
        if(c == '/') {
            j--;
            i = 0;
            continue;
        } 

        if(isdigit(c)) {
            i += c - '0';
            continue;
        }

        int bit = i + (j * BOARD_SIZE);
        set_bit(bs.all_pieces, bit);
        if(isupper(c)) set_bit(bs.get_bb(Colour::WHITE, PieceType::NONE), bit);
        else           set_bit(bs.get_bb(Colour::BLACK, PieceType::NONE), bit);

        switch(c) {
            case 'p': set_bit(bs.get_bb(Colour::BLACK, PieceType::PAWN),   bit); break;
            case 'P': set_bit(bs.get_bb(Colour::WHITE, PieceType::PAWN),   bit); break;
            case 'n': set_bit(bs.get_bb(Colour::BLACK, PieceType::KNIGHT), bit); break;
            case 'N': set_bit(bs.get_bb(Colour::WHITE, PieceType::KNIGHT), bit); break;
            case 'b': set_bit(bs.get_bb(Colour::BLACK, PieceType::BISHOP), bit); break;
            case 'B': set_bit(bs.get_bb(Colour::WHITE, PieceType::BISHOP), bit); break;
            case 'r': set_bit(bs.get_bb(Colour::BLACK, PieceType::ROOK),   bit); break;
            case 'R': set_bit(bs.get_bb(Colour::WHITE, PieceType::ROOK),   bit); break;
            case 'q': set_bit(bs.get_bb(Colour::BLACK, PieceType::QUEEN),  bit); break;
            case 'Q': set_bit(bs.get_bb(Colour::WHITE, PieceType::QUEEN),  bit); break;
            case 'k': set_bit(bs.get_bb(Colour::BLACK, PieceType::KING),    bit); break;
            case 'K': set_bit(bs.get_bb(Colour::WHITE, PieceType::KING),    bit); break;
            default: break;
        }

        i++;
    }

    bs.castling_rights = 0;
    std::string castling_config_str = components[FEN_CASTLING_CONFIG];
    if (castling_config_str != "-") {
        for (char c : castling_config_str) {
            switch (c) {
                case 'K': bs.castling_rights |= W_KING_CASTLE_MASK;  break;
                case 'Q': bs.castling_rights |= W_QUEEN_CASTLE_MASK; break;
                case 'k': bs.castling_rights |= B_KING_CASTLE_MASK;  break;
                case 'q': bs.castling_rights |= B_QUEEN_CASTLE_MASK; break;
                default: break;
            }
        }
    }

    // parse en passant target square, e.g. "e3" or "-"
    std::string ep_config_str = components[FEN_EN_PASSANT_CONFIG];
    if (ep_config_str == "-") {
        bs.en_passant_square = NULL_SQ;
    } else {
        int file = ep_config_str[0] - 'a';
        int rank = ep_config_str[1] - '0';
        int rank_from_top = BOARD_SIZE - rank;
        bs.en_passant_square = file + (rank_from_top * BOARD_SIZE);
    }

    // parse halfmove clock (number of halfmoves since last capture/pawn move)
    // optional field - defaults to 0 if omitted
    bs.half_move_count = components.size() > FEN_HALFMOVE_CONFIG
        ? static_cast<uint8_t>(std::stoi(components[FEN_HALFMOVE_CONFIG]))
        : 0;

    // parse fullmove number (increments after black's move)
    // optional field - defaults to 1 if omitted
    bs.full_move_count = components.size() > FEN_FULLMOVE_CONFIG
        ? static_cast<uint8_t>(std::stoi(components[FEN_FULLMOVE_CONFIG]))
        : 1;

    bs.pinned_sqs = calc_pinned_sqs();
    bs.hash = zhash(bs);
}

// ----------------------------------------------
// Move logic
// ----------------------------------------------

void BoardManager::make_move(const Move& move) {
    LOG_TRACE;
    LOG("Making move.from: " + to_string_coord(move.from) + 
        " move.to: " + to_string_coord(move.to)
    );
    history.push_back(bs); // copy and store history

    // As we process the making of a move we incrementally update the zobrist hash
    // of the board state. This is because it is more efficient then making all the
    // board state changes and then fully-recomputing the resulting hash. This is
    // possible because zhashing is based on XOR operations which cancel out (x XOR x := 0).
    // So to update the hash we: take the existing hash, XOR it with the prior hash key used
    // based on the prior board state element, update the board state, XOR the hash with the hash key
    // for the new board state element.

    // some aliases
    Colour side = bs.turn;
    uint64_t& h = bs.hash;

    if (bs.en_passant_square != NULL_SQ) {
        h ^= ZEP_FILE[bs.en_passant_square & (BOARD_SIZE - 1)];
    }
    bs.en_passant_square = NULL_SQ; // no hash XOR needed as en-passant file is NULL (i.e. h XOR 0)

    // update move counts
    bs.half_move_count++;
    if (side == Colour::BLACK) {
        ++bs.full_move_count;
    }

    // normal captures
    for (uint8_t c = 0; c < NUM_COLOURS; ++c) {
        for (uint8_t pt = 0; pt < NUM_PIECE_TYPES; ++pt) {
            Bitboard& bb = bs.get_bb(static_cast<Colour>(c), static_cast<PieceType>(pt));

            if (is_bit(bb, move.to)) {
                // update hash and clear bit of captured piece
                h ^= ZPIECE_SQ[c][pt][move.to];
                clear_bit(bb, move.to);

                // captures are an irreversible move -> reset counter
                bs.half_move_count = 0;

                // jump as only one piece can occupy move.to
                goto capture_done;
            }
        }
    }
    capture_done: // goto label

    // update the bitboard for the moved piece
    h ^= ZPIECE_SQ[static_cast<uint8_t>(side)][static_cast<uint8_t>(move.moved_piece)][move.from];
    h ^= ZPIECE_SQ[static_cast<uint8_t>(side)][static_cast<uint8_t>(move.moved_piece)][move.to];
    Bitboard& bb = bs.get_bb(side, move.moved_piece);
    clear_bit(bb, move.from);
    set_bit(bb, move.to);

    // pawn logic edge cases
    if(move.moved_piece == PieceType::PAWN) {
        int forward = side == Colour::WHITE ? BOARD_SIZE : -BOARD_SIZE;

        // pawn moves are irreversible -> reset counter
        bs.half_move_count = 0;

        // en-passant capture
        if(move.is_en_passant) {
            Bitboard& enemy_pawns = bs.get_bb(!side, PieceType::PAWN);
            Square captured_sq = move.to - forward;

            // update hash and clear the bit of the taken pawn
            h ^= ZPIECE_SQ[static_cast<uint8_t>(!side)][static_cast<uint8_t>(PieceType::PAWN)][captured_sq];
            clear_bit(enemy_pawns, captured_sq);
        }

        // making a double push
        if(abs(move.from - move.to) == 2 * BOARD_SIZE) {
            h ^= ZEP_FILE[bs.en_passant_square & (BOARD_SIZE - 1)]; // update enpassant file hash
            bs.en_passant_square = move.from + forward;
        }

        // promotion
        if(move.promotion_piece != PieceType::NONE) {
            // above we set the bit for move.to but we need to remove it again as pawn promotions
            // are an edge case and obviously are promoted to piece
            h ^= ZPIECE_SQ[static_cast<uint8_t>(side)][static_cast<uint8_t>(PieceType::PAWN)][move.to];
            clear_bit(bb, move.to);

            // set the bit for the promoted piece
            h ^= ZPIECE_SQ[static_cast<uint8_t>(side)][static_cast<uint8_t>(move.promotion_piece)][move.to];
            Bitboard& prom_bb = bs.get_bb(side, move.promotion_piece);
            set_bit(prom_bb, move.to);
        }
    }

    // castling
    h ^= ZCASTLING[bs.castling_rights & 0xF]; // cancel out old castling rights from hash
    if(move.q_castle) [[unlikely]] process_q_castle(side);
    if(move.k_castle) [[unlikely]] process_k_castle(side);
    update_castling_rights(move.from);
    update_castling_rights(move.to);
    h ^= ZCASTLING[bs.castling_rights & 0xF]; // encode new castling rights in hash

    update_poly_bbs();

    bs.turn = !bs.turn;
    h ^= ZSIDE; // hash for changing sides is always XOR'd

    bs.pinned_sqs = calc_pinned_sqs();

    ASSERT(bs.hash == zhash(bs), "Non-matching incremental hash and static hash calculations");
    LOG("Alternating turn, turn is now: " + to_string_colour(bs.turn));
}

void BoardManager::undo_move() {
    LOG_TRACE;

    bs = history.back();
    history.pop_back();
}

void inline BoardManager::update_castling_rights(Square move_pos) {
    // updating white castling rights
    if(bs.castling_rights & W_QUEEN_CASTLE_MASK || bs.castling_rights & W_KING_CASTLE_MASK) {
        if(move_pos == W_KING_START_POS) [[unlikely]] {
            bs.castling_rights &= ~W_QUEEN_CASTLE_MASK;
            bs.castling_rights &= ~W_KING_CASTLE_MASK;
            bs.half_move_count = 0;
        }
        if(move_pos == W_QUEEN_ROOK_START_POS) [[unlikely]] {
            bs.castling_rights &= ~W_QUEEN_CASTLE_MASK;
            bs.half_move_count = 0;
        }
        if(move_pos == W_KING_ROOK_START_POS)  [[unlikely]] {
            bs.castling_rights &= ~W_KING_CASTLE_MASK;
            bs.half_move_count = 0;
        }
    }

    // update black castling rights
    if(bs.castling_rights & B_QUEEN_CASTLE_MASK || bs.castling_rights & B_KING_CASTLE_MASK) {
        if(move_pos == B_KING_START_POS) [[unlikely]] {
            bs.castling_rights &= ~B_QUEEN_CASTLE_MASK;
            bs.castling_rights &= ~B_KING_CASTLE_MASK;
            bs.half_move_count = 0;
        }
        if(move_pos == B_QUEEN_ROOK_START_POS) [[unlikely]] {
            bs.castling_rights &= ~B_QUEEN_CASTLE_MASK;
            bs.half_move_count = 0;
        }
        if(move_pos == B_KING_ROOK_START_POS) [[unlikely]] {
            bs.castling_rights &= ~B_KING_CASTLE_MASK;
            bs.half_move_count = 0;
        }
    }
}

void inline BoardManager::update_poly_bbs() {
    Bitboard& white_pieces = bs.get_bb(Colour::WHITE, PieceType::NONE);
    white_pieces = bs.get_bb(Colour::WHITE, PieceType::PAWN) |
        bs.get_bb(Colour::WHITE, PieceType::PAWN) |
        bs.get_bb(Colour::WHITE, PieceType::KNIGHT) |
        bs.get_bb(Colour::WHITE, PieceType::BISHOP) |
        bs.get_bb(Colour::WHITE, PieceType::ROOK) |
        bs.get_bb(Colour::WHITE, PieceType::QUEEN) |
        bs.get_bb(Colour::WHITE, PieceType::KING);

    Bitboard& black_pieces = bs.get_bb(Colour::BLACK, PieceType::NONE);
    black_pieces = bs.get_bb(Colour::BLACK, PieceType::PAWN) |
        bs.get_bb(Colour::BLACK, PieceType::KNIGHT) |
        bs.get_bb(Colour::BLACK, PieceType::BISHOP) |
        bs.get_bb(Colour::BLACK, PieceType::ROOK) |
        bs.get_bb(Colour::BLACK, PieceType::QUEEN) |
        bs.get_bb(Colour::BLACK, PieceType::KING);

    bs.all_pieces = white_pieces | black_pieces;
}

void BoardManager::process_q_castle(Colour side) {
    LOG_TRACE;
    Square rook_from, rook_to, king_from, king_to;
    if(side == Colour::WHITE) {
        rook_from = W_QUEEN_ROOK_START_POS;
        rook_to   = W_QUEEN_ROOK_CASTLE_TO;
        king_from = W_KING_START_POS;
        king_to   = W_QUEEN_CASTLE_TO;
    } else {
        rook_from = B_QUEEN_ROOK_START_POS;
        rook_to   = B_QUEEN_ROOK_CASTLE_TO;
        king_from = B_KING_START_POS;
        king_to   = B_QUEEN_CASTLE_TO;
    }
    process_castle(rook_from, rook_to, king_from, king_to, side);
}

void BoardManager::process_k_castle(Colour side) {
    LOG_TRACE;
    Square rook_from, rook_to, king_from, king_to;
    if(side == Colour::WHITE) {
        rook_from = W_KING_ROOK_START_POS;
        rook_to   = W_KING_ROOK_CASTLE_TO;
        king_from = W_KING_START_POS;
        king_to   = W_KING_CASTLE_TO;
    } else {
        rook_from = B_KING_ROOK_START_POS;
        rook_to   = B_KING_ROOK_CASTLE_TO;
        king_from = B_KING_START_POS;
        king_to   = B_KING_CASTLE_TO;
    }
    process_castle(rook_from, rook_to, king_from, king_to, side);
}

void BoardManager::process_castle(
    Square rook_from, Square rook_to,
    Square king_from, Square king_to, Colour side
) {
    LOG_TRACE;
    Bitboard& king_bb = bs.get_bb(side, PieceType::KING);
    Bitboard& rook_bb = bs.get_bb(side, PieceType::ROOK);

    clear_bit(rook_bb, rook_from);
    set_bit(rook_bb, rook_to);
    clear_bit(king_bb, king_from);
    set_bit(king_bb, king_to);

    // update hashes for rook position
    int c = static_cast<uint8_t>(side);
    int rook_pt = static_cast<uint8_t>(PieceType::ROOK);
    bs.hash ^= ZPIECE_SQ[c][rook_pt][rook_from];
    bs.hash ^= ZPIECE_SQ[c][rook_pt][rook_to];
}

tracked_array<Move, MAX_NUM_MOVES> BoardManager::get_legal_moves(const tracked_array<Move, MAX_NUM_MOVES>& moves, bool in_check) {
    LOG_TRACE;
    LOGGER_OFF;

    /*
     * A two-step approach is used to check move legality:
     *  1. Any moves which would require excessive compute to check -> push to a buffer. Later
     *      we make each move in the buffer and check if it leaves the player in check, if not -> legal
     *  2. Other moves are legal and so no need to fully compute making the move.
     *
     * This is done to try and minimise the number of moves which you need to fully compute the: "make_move ->
     * is_in_check -> unmake_move" pattern
     */

    tracked_array<Move, MAX_NUM_MOVES> legal_moves;
    tracked_array<Move, MAX_NUM_MOVES> move_buff;
    Colour side = bs.turn;

    for (const Move& move : moves) {
        // check castle moves and skip if not valid
        if (move.q_castle && !valid_q_castle(move.from, side, in_check)) continue;
        if (move.k_castle && !valid_k_castle(move.from, side, in_check)) continue;

        // If the player is in check, or we're evaluating king moves, buffer it to fully compute later
        if (in_check || move.moved_piece == PieceType::KING) {
            move_buff.push_back(move);
            continue;
        }

        // push pinned piece moves for fully compute check
        if (is_pinned(move.from)) {
            move_buff.push_back(move);
            continue;
        }

        // push enpassant moves to the buffer to fully compute
        // this is to account for en-passants causing two pawns to disappear from the same rank, which could
        // result in a discovered check on the player for horizontal moving pieces (rooks, queens)
        //
        // could optimise this by shooting a ray to check explicitly instead of fully computing, but such instances
        // are rare and so likely induce little overhead. -> refactor and implement optimisation if this branch
        // induces signifcant overhead.
        if (move.is_en_passant) {
            move_buff.push_back(move);
            continue;
        }

        // Otherwise, move is legal
        legal_moves.push_back(move);
    }

    // for all buffered moves, check which are legal
    for(const Move& move : move_buff) {
        make_move(move);
        if (!is_in_check(side)) legal_moves.push_back(move);
        undo_move();
    }

    // Note: by buffering a moves which need to be fully computed, we are able to quickly
    // operate upon the cached pinned_sqs in the board state.

    LOGGER_ON;
    return legal_moves;
}

tracked_array<Move, MAX_NUM_MOVES> BoardManager::get_legal_moves(const tracked_array<Move, MAX_NUM_MOVES>& moves) {
    return get_legal_moves(moves, is_in_check(bs.turn));
}

bool BoardManager::is_pinned(Square from) {
    for(Square pinned_sq : bs.pinned_sqs) {
        if(from == pinned_sq) return true;
    }
    return false;
}

tracked_array<Square, BOARD_SIZE> BoardManager::calc_pinned_sqs() {
    tracked_array<Square, BOARD_SIZE> pinned_sqs;
    Colour side = bs.turn;

    Bitboard king_bb = bs.get_bb(side, PieceType::KING);
    Square king_sq = pop_lsb(king_bb);

    Bitboard own_pieces   = bs.get_bb(side, PieceType::NONE);
    Bitboard enemy_rooks  = bs.get_bb(!side, PieceType::ROOK);
    Bitboard enemy_bishops= bs.get_bb(!side, PieceType::BISHOP);
    Bitboard enemy_queens = bs.get_bb(!side, PieceType::QUEEN);
    Bitboard all_pieces   = bs.all_pieces;

    auto check_pins = [this, king_sq, own_pieces, enemy_queens, all_pieces](std::vector<std::pair<int, Bitboard>>& move_directions, Bitboard enemy_bb, tracked_array<Square, BOARD_SIZE>& _pinned_sqs){
        for (auto [dir, _] : move_directions) {
            Square sq = king_sq + dir;
            int candidate = -1;

            while (sq >= 0 && sq < 64 && same_line(king_sq, sq, dir)) {
                Bitboard mask = 1ULL << sq;

                if (all_pieces & mask) {
                    if (own_pieces & mask) {
                        if (candidate == -1) {
                            candidate = sq; // first friendly piece in line
                        } else break; // two blockers = no pin
                    } else {
                        // Enemy piece?
                        if ((enemy_bb & mask) || (enemy_queens & mask)) {
                            if (candidate != -1) _pinned_sqs.push_back(candidate);
                        }
                        break; // ray blocked either way
                    }
                }
                sq += dir;
            }
        }
    };

    check_pins(ROOK_DIRECTIONS, enemy_rooks, pinned_sqs);
    check_pins(BISHOP_DIRECTIONS, enemy_bishops, pinned_sqs);
    return pinned_sqs;
}

inline bool BoardManager::same_line(Square from, Square to, int dir) {
    Square from_rank = from / 8, from_file = from % 8;
    Square to_rank   = to / 8,   to_file   = to % 8;

    switch (dir) {
        case  1: case -1: return from_rank == to_rank;       // horizontal
        case  8: case -8: return from_file == to_file;       // vertical
        case  9: case -9: return (from_rank - from_file) == (to_rank - to_file); // diag NW -> SE (vice versa)
        case  7: case -7: return (from_rank + from_file) == (to_rank + to_file); // diag SW -> NE (vice versa)
        default: return false;
    }
}

bool BoardManager::valid_q_castle(Square move_from, Colour side, bool in_check) {
    LOG_TRACE;

    if (in_check) return false;

    Square rook_pos = side == Colour::WHITE ? W_QUEEN_ROOK_START_POS : B_QUEEN_ROOK_START_POS;
    return valid_castle(move_from, rook_pos, side, false);
}

bool BoardManager::valid_k_castle(Square move_from, Colour side, bool in_check) {
    LOG_TRACE;

    if (in_check) return false;

    Square rook_pos = side == Colour::WHITE ? W_KING_ROOK_START_POS : B_KING_ROOK_START_POS;
    return valid_castle(move_from, rook_pos, side, true);

}

// NOTE: does not consider if the king is already in check such should be precomputed
bool BoardManager::valid_castle(Square move_from, Square rook_pos, Colour side, bool king_castle) {
    LOG_TRACE;

    // Check that no pieces are between king and rook
    Bitboard delta_bb = get_delta_bb(move_from, rook_pos);
    if ((delta_bb & bs.all_pieces) != 0) return false;

    Square king_from = move_from;
    Square king_to   = king_castle ? move_from + 2 : move_from - 2;
    std::array<Square, 3> king_path;

    king_castle
        ? king_path = {king_from, static_cast<Square>(king_from + 1), king_to}
        : king_path = {king_from, static_cast<Square>(king_from - 1), king_to};

    const Bitboard& enemy_knights = bs.get_bb(!side, PieceType::KNIGHT);
    const Bitboard& enemy_king    = bs.get_bb(!side, PieceType::KING);
    const Bitboard& enemy_pawns   = bs.get_bb(!side, PieceType::PAWN);
    const Bitboard& enemy_bishops = bs.get_bb(!side, PieceType::BISHOP);
    const Bitboard& enemy_rooks   = bs.get_bb(!side, PieceType::ROOK);
    const Bitboard& enemy_queens  = bs.get_bb(!side, PieceType::QUEEN);

    const Bitboard enemy_sliders_rook  = enemy_rooks | enemy_queens;
    const Bitboard enemy_sliders_bishop= enemy_bishops | enemy_queens;

    const Bitboard enemy_occupancy     = bs.get_bb(!side, PieceType::NONE);
    const Bitboard own_occupancy       = bs.get_bb(side, PieceType::NONE);
    const Bitboard occupancy           = bs.all_pieces;

    // Check each square for attacks
    for (Square sq : king_path) {
        const Bitboard sq_bb = 1ULL << sq;

        // Non-sliding pieces
        if ((KNIGHT_ATTACKS[sq] & enemy_knights) != 0) return false;
        if ((KING_ATTACKS[sq]   & enemy_king)    != 0) return false;
        if ((side == Colour::WHITE ? PAWN_ATTACKS_B[sq] : PAWN_ATTACKS_W[sq]) & enemy_pawns) return false;

        // Sliding pieces
        if ((get_bishop_attack_sqs(enemy_sliders_bishop, own_occupancy, occupancy) & sq_bb) != 0) return false;
        if ((get_rook_attack_sqs(  enemy_sliders_rook,   own_occupancy, occupancy) & sq_bb) != 0) return false;
    }

    return true;
}


// helper lambda which construct a bb where all values are true between two values
Bitboard BoardManager::get_delta_bb(Square a, Square b) {
    LOG_TRACE;
    Square from = std::max(a, b);
    Square to = std::min(a, b);

    uint64_t upper_mask = (1ULL << from) - 1;
    uint64_t lower_mask = (1ULL << (to + 1)) - 1;
    uint64_t mask = upper_mask & ~lower_mask;

    return Bitboard(mask);
}

// ----------------------------------------------
// Game State checkers
// ----------------------------------------------

GameState BoardManager::get_game_state() {
    if (is_fifty_move_rule()) {
        return GameState::DRAW;
    }

    bool in_check = is_in_check(bs.turn);
    auto legal_moves = get_legal_moves(get_moves(bs.turn, bs));

    if (legal_moves.empty()) {
        return in_check ? GameState::CHECKMATE : GameState::DRAW; // checkmate or stalemate
    }

    if (is_three_fold_repetition() || is_insufficient_material()) [[unlikely]] {
        return GameState::DRAW;
    }

    if (in_check) {
        return GameState::CHECK;
    }

    return GameState::UNFINISHED;
}

bool BoardManager::is_fifty_move_rule() const {
    return bs.half_move_count >= 100;
}

bool BoardManager::is_three_fold_repetition() const {
    if (bs.half_move_count >= 8) { // 3-fold repetition only possible after 8 half moves since last reset

        // slice the history vector about lower and upper bounds for which a 3-fold repetition could occur.
        // this is explicitly the moves between (inclusive) the last irreversible move (i.e. half move count) and
        // the current move. We can also just check every other move as a matching hash could only
        // be found on the previous move of the current side
        //
        // We then count matching hash occurrences to the current board state hash
        // if the number of occurrences exceeds 3 -> 3-fold repetition
        //
        // Note: we take the upper bound as the previous move of the current side as `history.size().hash`
        // vacuously always matches the `current_hash` (they're the same thing). So init the number of occurences
        // to 1 and don't search the last element in the vector.

        uint64_t current_hash = bs.hash;
        int occurrences = 1;

        int lower_bound = static_cast<int>(history.size()) - bs.half_move_count;
        int upper_bound = static_cast<int>(history.size()) - 2;
        for (int i = lower_bound; i <= upper_bound; i += 2) {
            if (history[i].hash == current_hash) {
                occurrences++;
                if (occurrences >= 3) return true; // 3-fold repetition
            }
        }
    }

    return false;
}

bool BoardManager::is_insufficient_material() const {
    // NOTE: king + bishop vs. king + bishop is not technically insufficient material
    // however so long as both players possess a few brain cells it is always a draw
    // in practise. So we say it is insufficient material. It's not worth the extra compute
    // and effort for STRICT modelling of the rules

    uint8_t white_piece_count = std::popcount(bs.get_bb(Colour::WHITE, PieceType::NONE));
    uint8_t black_piece_count = std::popcount(bs.get_bb(Colour::BLACK, PieceType::NONE));

    // K vs. K
    if (white_piece_count == 1 && black_piece_count == 1) [[unlikely]] return true;

    bool white_heavy = bs.get_bb(Colour::WHITE, PieceType::QUEEN)  ||
                        bs.get_bb(Colour::WHITE, PieceType::ROOK)  ||
                        bs.get_bb(Colour::WHITE, PieceType::PAWN);
    bool black_heavy = bs.get_bb(Colour::BLACK, PieceType::QUEEN)  ||
                        bs.get_bb(Colour::BLACK, PieceType::ROOK)  ||
                        bs.get_bb(Colour::BLACK, PieceType::PAWN);

    if (white_heavy || black_heavy) return false;

    uint8_t white_minor_piece_count = std::popcount(bs.get_bb(Colour::WHITE, PieceType::BISHOP)) + std::popcount(bs.get_bb(Colour::WHITE, PieceType::KNIGHT));
    uint8_t black_minor_piece_count = std::popcount(bs.get_bb(Colour::BLACK, PieceType::BISHOP)) + std::popcount(bs.get_bb(Colour::BLACK, PieceType::KNIGHT));

    if (white_minor_piece_count <= 1 && black_minor_piece_count <= 1) return true;

    return false;
}

bool BoardManager::is_in_check(Colour side) const {
    LOG_TRACE;

    // check for sliding piece checks
    // could be optamised with magic bitboards
    Bitboard own_pieces   = bs.get_bb(!side, PieceType::NONE);
    Bitboard king_bb      = bs.get_bb(side, PieceType::KING);
    Bitboard occupancy    = bs.all_pieces;

    // queen attacks are bundled in with the below
    Bitboard bishop_attacks = get_bishop_attack_sqs(bs.get_bb(!side, PieceType::BISHOP) | bs.get_bb(!side, PieceType::QUEEN), own_pieces, occupancy);
    if((bishop_attacks & king_bb) != 0) return true;
    Bitboard rook_attacks     = get_rook_attack_sqs  (bs.get_bb(!side, PieceType::ROOK) | bs.get_bb(!side, PieceType::QUEEN), own_pieces, occupancy);
    if((rook_attacks & king_bb) != 0) return true;

    // check for attack array
    Square king_sq = pop_lsb(king_bb);
    if (KNIGHT_ATTACKS[king_sq] & bs.get_bb(!side, PieceType::KNIGHT)) return true;
    if (KING_ATTACKS[king_sq]   & bs.get_bb(!side, PieceType::KING))   return true;
    if (side == Colour::WHITE
        ? PAWN_ATTACKS_B[king_sq] & bs.get_bb(Colour::BLACK, PieceType::PAWN)
        : PAWN_ATTACKS_W[king_sq] & bs.get_bb(Colour::WHITE, PieceType::PAWN)
    ) return true;
    
    return false;
}


// ----------------------------------------------
// Getters
// ----------------------------------------------

BoardState& BoardManager::get_bs() {
    return bs;
}