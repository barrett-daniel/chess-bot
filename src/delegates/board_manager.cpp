#include "board_manager.hpp"

#include <sstream>
#include <utility>

#include "../utility/logger.hpp"
#include "../pieces/pieces.hpp"

// ----------------------------------------------
// Constructor
// ----------------------------------------------

BoardManager::BoardManager(const std::string fen) {
    LOG_TRACE;

    // decompose FEN into component sections
    std::stringstream ss(fen);
    std::vector<std::string> components;
    std::string elem;
    while(ss >> elem) components.push_back(elem);
    ASSERT(components.size() <= 6, "Malformed FEN: too many arguments provided");
    ASSERT(components.size() >= 4, "Malformed FEN: too few arguments provided");

    // determine whose first to move
    std::string turn_config_str = components[FEN_TURN_CONFIG];
    bs.turn = turn_config_str == "w" ? WHITE : BLACK;

    // initalise board data
    std::string board_config_str = components[FEN_BOARD_CONFIG];
    int i = 0, j = 0;
    for(char c : board_config_str) {
        std::string c_str(1, c);
        if(c == '/') {
            j++;
            i = 0;
            continue;
        } 

        if(isdigit(c)) {
            i += c - '0';
            continue;
        }

        int bit = i + (j * BOARD_SIZE);
        set_bit(bs.all_pieces, bit);
        if(isupper(c)) set_bit(bs.w_pieces, bit);
        else           set_bit(bs.b_pieces, bit);

        switch(c) {
            case 'p': set_bit(bs.b_pawns,   bit); break;
            case 'P': set_bit(bs.w_pawns,   bit); break;
            case 'n': set_bit(bs.b_knights, bit); break;
            case 'N': set_bit(bs.w_knights, bit); break;
            case 'b': set_bit(bs.b_bishops, bit); break;
            case 'B': set_bit(bs.w_bishops, bit); break;
            case 'r': set_bit(bs.b_rooks,   bit); break;
            case 'R': set_bit(bs.w_rooks,   bit); break;
            case 'q': set_bit(bs.b_queens,  bit); break;
            case 'Q': set_bit(bs.w_queens,  bit); break;
            case 'k': set_bit(bs.b_king,    bit); break;
            case 'K': set_bit(bs.w_king,    bit); break;
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
        bs.en_passant_square = -1;
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
}

// ----------------------------------------------
// Move logic
// ----------------------------------------------

void BoardManager::make_move(const Move& move) {
    LOG_TRACE;
    LOG("Making move.from: " + to_string_coord(move.from) + 
        " move.to: " + to_string_coord(move.to)
    );

    Colour side = bs.turn;
    history.push(bs);
    bs.en_passant_square = -1;

    if (side == BLACK) {
        ++bs.full_move_count;
    }

    // reset all bitboards
    for(Bitboard& bb : bs.piece_bbs()) clear_bit(bb, move.to);

    // update moved piece bb
    Bitboard& bb = bs.get_bb(side, move.moved_piece);
    clear_bit(bb, move.from);
    set_bit(bb, move.to);

    // pawn logic
    if(move.moved_piece == PieceType::PAWN) {
        int forward = side == WHITE ? -BOARD_SIZE : BOARD_SIZE;

        // en-passant capture
        if(move.is_en_passant) {
            Bitboard& enemy_pawns = bs.get_bb(!side, PieceType::PAWN);
            clear_bit(enemy_pawns, move.to - forward);
        }

        // making a double push
        if(abs(move.from - move.to) == 2 * BOARD_SIZE) {
            bs.en_passant_square = move.from + forward;
        }

        // promotion
        if(move.promotion_piece != PieceType::NONE) {
            clear_bit(bb, move.to);
            Bitboard& prom_bb = bs.get_bb(side, move.promotion_piece);
            set_bit(prom_bb, move.to);
        }
    }

    // castling
    if(move.q_castle) [[unlikely]] process_q_castle(side);
    if(move.k_castle) [[unlikely]] process_k_castle(side);
    update_castling_rights(move.from);
    update_castling_rights(move.to);

    update_poly_bbs();
    bs.turn = !bs.turn;
    bs.pinned_sqs = calc_pinned_sqs();
    LOG("Alternating turn, turn is now: " + to_string(bs.turn));
}

void BoardManager::undo_move() {
    LOG_TRACE;

    bs = history.top();
    history.pop();
}

void inline BoardManager::update_castling_rights(int move_pos) {
    // updating white castling rights
    if(bs.castling_rights & W_QUEEN_CASTLE_MASK || bs.castling_rights & W_KING_CASTLE_MASK) {
        if(move_pos == W_KING_START_POS) [[unlikely]] {
            bs.castling_rights &= ~W_QUEEN_CASTLE_MASK;
            bs.castling_rights &= ~W_KING_CASTLE_MASK;
        }
        if(move_pos == W_QUEEN_ROOK_START_POS) [[unlikely]] bs.castling_rights &= ~W_QUEEN_CASTLE_MASK;
        if(move_pos == W_KING_ROOK_START_POS)  [[unlikely]] bs.castling_rights &= ~W_KING_CASTLE_MASK;
    }

    // update black castling rights
    if(bs.castling_rights & B_QUEEN_CASTLE_MASK || bs.castling_rights & B_KING_CASTLE_MASK) {
        if(move_pos == B_KING_START_POS) [[unlikely]] {
            bs.castling_rights &= ~B_QUEEN_CASTLE_MASK;
            bs.castling_rights &= ~B_KING_CASTLE_MASK;
        }
        if(move_pos == B_QUEEN_ROOK_START_POS) [[unlikely]] bs.castling_rights &= ~B_QUEEN_CASTLE_MASK;
        if(move_pos == B_KING_ROOK_START_POS)  [[unlikely]] bs.castling_rights &= ~B_KING_CASTLE_MASK;
    }
}

void inline BoardManager::update_poly_bbs() {
    bs.w_pieces   = bs.w_pawns | bs.w_knights | bs.w_bishops | 
                    bs.w_rooks | bs.w_queens | bs.w_king;
    bs.b_pieces   = bs.b_pawns | bs.b_knights | bs.b_bishops | 
                    bs.b_rooks | bs.b_queens | bs.b_king;
    bs.all_pieces = bs.w_pieces | bs.b_pieces;
}

void BoardManager::process_q_castle(Colour side) {
    LOG_TRACE;
    int rook_from, rook_to, king_from, king_to;
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
    int rook_from, rook_to, king_from, king_to;
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
    int rook_from, int rook_to, 
    int king_from, int king_to, Colour side
) {
    LOG_TRACE;
    Bitboard& king_bb = bs.get_bb(side, PieceType::KING);
    Bitboard& rook_bb = bs.get_bb(side, PieceType::ROOK);

    clear_bit(rook_bb, rook_from);
    set_bit(rook_bb, rook_to);
    clear_bit(king_bb, king_from);
    set_bit(king_bb, king_to);
}

std::vector<Move> BoardManager::get_legal_moves(const std::vector<Move>& moves) {
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

    std::vector<Move> legal_moves;
    std::vector<Move> move_buff;
    Colour side = bs.turn;

    bool in_check = is_in_check(side);
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

bool BoardManager::is_pinned(int from) {
    for(int pinned_sq : bs.pinned_sqs) {
        if(from == pinned_sq) return true;
    }
    return false;
}

std::vector<int> BoardManager::calc_pinned_sqs() {
    std::vector<int> pinned_sqs;
    Colour side = bs.turn;

    Bitboard king_bb = bs.get_bb(side, PieceType::KING);
    int king_sq = pop_lsb(king_bb);

    Bitboard own_pieces   = bs.get_bb(side, PieceType::NONE);
    Bitboard enemy_rooks  = bs.get_bb(!side, PieceType::ROOK);
    Bitboard enemy_bishops= bs.get_bb(!side, PieceType::BISHOP);
    Bitboard enemy_queens = bs.get_bb(!side, PieceType::QUEEN);
    Bitboard all_pieces   = bs.all_pieces;

    auto check_pins = [this, king_sq, own_pieces, enemy_queens, all_pieces](std::vector<std::pair<int, Bitboard>>& move_directions, Bitboard enemy_bb, std::vector<int>& pinned_sqs){
        for (auto [dir, _] : move_directions) {
            int sq = king_sq + dir;
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
                            if (candidate != -1) pinned_sqs.push_back(candidate);
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

inline bool BoardManager::same_line(int from, int to, int dir) {
    int from_rank = from / 8, from_file = from % 8;
    int to_rank   = to / 8,   to_file   = to % 8;

    switch (dir) {
        case  1: case -1: return from_rank == to_rank;       // horizontal
        case  8: case -8: return from_file == to_file;       // vertical
        case  9: case -9: return (from_rank - from_file) == (to_rank - to_file); // diag NW -> SE (vice versa)
        case  7: case -7: return (from_rank + from_file) == (to_rank + to_file); // diag SW -> NE (vice versa)
        default: return false;
    }
}

bool BoardManager::valid_q_castle(int move_from, Colour side, bool in_check) {
    LOG_TRACE;

    if (in_check) return false;

    int rook_pos = side == WHITE ? W_QUEEN_ROOK_START_POS : B_QUEEN_ROOK_START_POS;
    return valid_castle(move_from, rook_pos, side, false);
}

bool BoardManager::valid_k_castle(int move_from, Colour side, bool in_check) {
    LOG_TRACE;

    if (in_check) return false;

    int rook_pos = side == WHITE ? W_KING_ROOK_START_POS : B_KING_ROOK_START_POS;
    return valid_castle(move_from, rook_pos, side, true);

}

// NOTE: does not consider if the king is already in check such should be precomputed
bool BoardManager::valid_castle(int move_from, int rook_pos, Colour side, bool king_castle) {
    LOG_TRACE;

    // Check that no pieces are between king and rook
    Bitboard delta_bb = get_delta_bb(move_from, rook_pos);
    if ((delta_bb & bs.all_pieces) != 0) return false;

    int king_from = move_from;
    int king_to   = king_castle ? move_from + 2 : move_from - 2;
    std::vector<int> king_path;

    king_castle
        ? king_path = {king_from, king_from + 1, king_to}
        : king_path = {king_from, king_from - 1, king_to};

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

    // Check each square for attacks
    for (int sq : king_path) {
        const Bitboard sq_bb = 1ULL << sq;

        // Non-sliding pieces
        if ((KNIGHT_ATTACKS[sq] & enemy_knights) != 0) return false;
        if ((KING_ATTACKS[sq]   & enemy_king)    != 0) return false;
        if ((side == Colour::WHITE ? PAWN_ATTACKS_B[sq] : PAWN_ATTACKS_W[sq]) & enemy_pawns) return false;

        // Sliding pieces
        if ((get_bishop_attack_sqs(enemy_sliders_bishop, own_occupancy, enemy_occupancy) & sq_bb) != 0) return false;
        if ((get_rook_attack_sqs(  enemy_sliders_rook,   own_occupancy, enemy_occupancy) & sq_bb) != 0) return false;
    }

    return true;
}


// helper lambda which construct a bb where all values are true between two values
Bitboard BoardManager::get_delta_bb(int a, int b) {
    LOG_TRACE;
    int from = std::max(a, b);
    int to = std::min(a, b);

    uint64_t upper_mask = (1ULL << from) - 1;
    uint64_t lower_mask = (1ULL << (to + 1)) - 1;
    uint64_t mask = upper_mask & ~lower_mask;

    return Bitboard(mask);
}

// ----------------------------------------------
// Game State checkers
// ----------------------------------------------

bool BoardManager::is_in_check(Colour side) {
    LOG_TRACE;

    // check for sliding piece checks
    // could be optamised with magic bitboards
    Bitboard own_pieces   = bs.get_bb(!side, PieceType::NONE);
    Bitboard enemy_pieces = bs.get_bb(side, PieceType::NONE);
    Bitboard king_bb      = bs.get_bb(side, PieceType::KING);

    // queen attacks are bundled in with the below
    Bitboard bishop_attacks = get_bishop_attack_sqs(bs.get_bb(!side, PieceType::BISHOP) | bs.get_bb(!side, PieceType::QUEEN), own_pieces, enemy_pieces);
    if((bishop_attacks & king_bb) != 0) return true;
    Bitboard rook_attacks     = get_rook_attack_sqs  (bs.get_bb(!side, PieceType::ROOK) | bs.get_bb(!side, PieceType::QUEEN), own_pieces, enemy_pieces);
    if((rook_attacks & king_bb) != 0) return true;

    // check for attack array
    int king_sq = pop_lsb(king_bb);
    if (KNIGHT_ATTACKS[king_sq] & bs.get_bb(!side, PieceType::KNIGHT)) return true;
    if (KING_ATTACKS[king_sq]   & bs.get_bb(!side, PieceType::KING))   return true;
    if (side == Colour::WHITE ? PAWN_ATTACKS_B[king_sq] & bs.b_pawns : PAWN_ATTACKS_W[king_sq] & bs.w_pawns) return true;
    
    return false;
}

bool BoardManager::is_checkmate(Colour side) {
    LOG_TRACE;
    
    if (!is_in_check(side)) return false;

    std::vector<Move> moves = get_moves(side, bs);
    std::vector<Move> legal_moves = get_legal_moves(moves);
    return legal_moves.empty();
}

bool BoardManager::is_stalemate(Colour side) {
    LOG_TRACE;

    bool no_legal_moves = get_legal_moves(get_moves(side, bs)).empty();
    bool not_in_check = !is_in_check(side);
    return no_legal_moves & not_in_check;
}

// ----------------------------------------------
// Getters
// ----------------------------------------------

BoardState& BoardManager::get_bs() {
    return bs;
}