#include "board_state.hpp"

#include "util/bit_utils.hpp"

BoardState::BoardState() :
    bbs({}),
    all_pieces(0),
    turn(Colour::WHITE),
    castling_rights(0),
    en_passant_square(NULL_SQ),
    pinned_sqs({}),
    full_move_count(0),
    half_move_count(0),
    hash(0),
    edata({})
{}

Bitboard& BoardState::get_bb(Colour side, PieceType type) {
    return bbs[static_cast<u8>(side) * 7 + static_cast<u8>(type)];
}

Bitboard BoardState::get_bb(Colour side, PieceType type) const {
    return bbs[static_cast<u8>(side) * 7 + static_cast<u8>(type)];
}

bool BoardState::is_capture(const Move& move) const {
    const Bitboard& enemy_pieces = get_bb(!turn, PieceType::NONE);
    return is_bit(enemy_pieces, move.to);
}

std::string BoardState::to_string() const {
    // [white][black] piece characters, indexed by PieceType value (0-5)
    static constexpr std::array<std::array<char, 6>, 2> piece_chars = {{
        {'P', 'N', 'B', 'R', 'Q', 'K'},  // white
        {'p', 'n', 'b', 'r', 'q', 'k'}   // black
    }};

    std::string out;
    out.reserve(8 * 8 * 2 + 8 * 4 + 20);

    out += "  a b c d e f g h\n";
    for (int rank = 7; rank >= 0; --rank) {
        out += std::to_string(rank + 1);
        out += ' ';

        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            Bitboard mask = static_cast<Bitboard>(1) << sq;
            char piece_char = '.';

            for (int colour = 0; colour < 2 && piece_char == '.'; ++colour) {
                for (int pt = 0; pt < 6; ++pt) {
                    Bitboard bb = get_bb(static_cast<Colour>(colour), static_cast<PieceType>(pt));
                    if (bb & mask) {
                        piece_char = piece_chars[colour][pt];
                        break;
                    }
                }
            }

            out += piece_char;
            out += ' ';
        }

        out += ' ';
        out += std::to_string(rank + 1);
        out += '\n';
    }
    out += "  a b c d e f g h\n";

    return out;
}

std::string BoardState::to_fen() const {
    std::string fen;
    fen.reserve(80);

    // --- 1. Piece placement ---
    for (int rank = 7; rank >= 0; --rank) {
        int empty_count = 0;
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            Bitboard mask = static_cast<Bitboard>(1) << sq;
            char piece_char = '\0';

            for (int colour = 0; colour < 2 && piece_char == '\0'; ++colour) {
                for (int pt = 0; pt < 6; ++pt) {
                    Bitboard bb = get_bb(static_cast<Colour>(colour), static_cast<PieceType>(pt));
                    if (bb & mask) {
                        static constexpr std::array<std::array<char, 6>, 2> piece_chars = {{
                            {'P', 'N', 'B', 'R', 'Q', 'K'},  // white
                            {'p', 'n', 'b', 'r', 'q', 'k'}   // black
                        }};
                        piece_char = piece_chars[colour][pt];
                        break;
                    }
                }
            }

            if (piece_char == '\0') {
                ++empty_count;
            } else {
                if (empty_count > 0) {
                    fen += std::to_string(empty_count);
                    empty_count = 0;
                }
                fen += piece_char;
            }
        }
        if (empty_count > 0) {
            fen += std::to_string(empty_count);
        }
        if (rank > 0) {
            fen += '/';
        }
    }

    // --- 2. Active colour ---
    fen += ' ';
    fen += (turn == Colour::WHITE) ? 'w' : 'b';

    // --- 3. Castling rights ---
    // ASSUMPTION: bit0 = white kingside (K), bit1 = white queenside (Q),
    // bit2 = black kingside (k), bit3 = black queenside (q).
    // Adjust the mask/order below if your actual bit layout differs.
    fen += ' ';
    std::string castling;
    if (castling_rights & W_KING_CASTLE_MASK)  castling += 'K';
    if (castling_rights & W_QUEEN_CASTLE_MASK) castling += 'Q';
    if (castling_rights & B_KING_CASTLE_MASK)  castling += 'k';
    if (castling_rights & B_QUEEN_CASTLE_MASK) castling += 'q';
    fen += castling.empty() ? "-" : castling;

    // --- 4. En passant target square ---
    // ASSUMPTION: Square is an integer-like type where value == rank*8 + file
    // (matching the indexing used in to_string()/get_bb loops above),
    // and NULL_SQ is the sentinel for "no en passant square".
    fen += ' ';
    if (en_passant_square == NULL_SQ) {
        fen += '-';
    } else {
        int sq = static_cast<int>(en_passant_square);
        int file = sq % 8;
        int rank = sq / 8;
        fen += static_cast<char>('a' + file);
        fen += static_cast<char>('1' + rank);
    }

    // --- 5. Halfmove clock ---
    fen += ' ';
    fen += std::to_string(static_cast<int>(half_move_count));

    // --- 6. Fullmove number ---
    // ASSUMPTION: full_move_count already follows FEN convention (starts at 1
    // for the first move). If your engine starts it at 0 internally, you may
    // want `std::to_string(full_move_count + 1)` instead.
    fen += ' ';
    fen += std::to_string(full_move_count);

    return fen;
}