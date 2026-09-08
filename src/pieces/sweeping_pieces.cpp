#include "sweeping_pieces.hpp"
#include "../utility/logger.hpp"

// ------------------------------------
// Private Function Prototypes
// ------------------------------------
tracked_array<Move, MAX_NUM_MOVES> get_sweep_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces,
    std::vector<std::pair<int, Bitboard>> directions,
    PieceType type
);

Bitboard get_sweep(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces,
    const Bitboard& edge_mask,
    int dir
);

void encode_sweep_move_data(
    Bitboard bb, 
    Square from,
    tracked_array<Move, MAX_NUM_MOVES>& moves, 
    PieceType type
);

// ------------------------------------
// Public functions
// ------------------------------------

Bitboard get_rook_attack_sqs(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    
    Bitboard bb = 0;

    while(from_bb) {
        Square from = pop_lsb(from_bb);
        Bitboard piece_bb = 1ULL << from;

        for(auto [dir, mask] : ROOK_DIRECTIONS) {
            bb |= get_sweep(piece_bb, own_pieces, enemy_pieces, mask, dir);
        }
    }

    return bb;
}

Bitboard get_bishop_attack_sqs(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    
    Bitboard bb = 0;

    while(from_bb) {
        Square from = pop_lsb(from_bb);
        Bitboard piece_bb = 1ULL << from;

        for(auto [dir, mask] : BISHOP_DIRECTIONS) {
            bb |= get_sweep(piece_bb, own_pieces, enemy_pieces, mask, dir);
        }
    }

    return bb;
}

Bitboard get_queen_attack_sqs(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    
    return  get_bishop_attack_sqs(from_bb, own_pieces, enemy_pieces) |
            get_rook_attack_sqs(from_bb, own_pieces, enemy_pieces);
}

tracked_array<Move, MAX_NUM_MOVES> get_rook_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    

    return get_sweep_moves(
        from_bb, own_pieces, enemy_pieces, 
        ROOK_DIRECTIONS, PieceType::ROOK
    );
}

tracked_array<Move, MAX_NUM_MOVES> get_bishop_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    

    return get_sweep_moves(
        from_bb, own_pieces, enemy_pieces, 
        BISHOP_DIRECTIONS, PieceType::BISHOP
    );
}

tracked_array<Move, MAX_NUM_MOVES> get_queen_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces
) {
    
    // get both rook and bishop moves -> combine them
    auto bishop_moves = get_sweep_moves(
        from_bb, own_pieces, enemy_pieces, 
        BISHOP_DIRECTIONS, PieceType::QUEEN
    );
    auto rook_moves = get_sweep_moves(
        from_bb, own_pieces, enemy_pieces, 
        ROOK_DIRECTIONS, PieceType::QUEEN
    );

    rook_moves.insert(rook_moves.end(), bishop_moves.begin(), bishop_moves.end());
    return rook_moves;
}

// ------------------------------------
// Private Function Definitions
// ------------------------------------

tracked_array<Move, MAX_NUM_MOVES> get_sweep_moves(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces,
    std::vector<std::pair<int, Bitboard>> directions,
    PieceType type
) {
    tracked_array<Move, MAX_NUM_MOVES> moves;

    // do each separately
    while(from_bb) {
        Square from = pop_lsb(from_bb);
        Bitboard piece_bb = 1ULL << from;

        for(auto [dir, mask] : directions) {
            Bitboard bb = get_sweep(piece_bb, own_pieces, enemy_pieces, mask, dir);
            encode_sweep_move_data(bb, from, moves, type);
        }
    }

    return moves;
}

// A bb sweep is generated like so:
// 1. Initalise a bitboard for all attacks
// 2. Initalise a temp bitboard representing the next attack which will be added to the attacks
// 3. If the next attack is on the edge_mask break (prevent wrap around)
// 4. Traverse the board in the given dir
// 5. If the next attack is off the board, or 'into' their own piece, do not add the attack and break
// 6. Add the next attack to the attacks_bb
// 7. If the next attack is attacking an enemy piece -> break (having already included the capture as a valid attack)
Bitboard get_sweep(
    Bitboard from_bb,
    const Bitboard& own_pieces,
    const Bitboard& enemy_pieces,
    const Bitboard& edge_mask,
    int dir
) {
    Bitboard attacks = 0;
    Bitboard temp = from_bb;

    while(true) {
        if((temp & edge_mask) != 0) break;

        temp = shift_dir(temp, dir);

        if(temp == 0) break;
        if((temp & own_pieces) != 0) break;

        attacks |= temp;

        if((temp & enemy_pieces) != 0) break;
    }

    return attacks;
}

void encode_sweep_move_data(Bitboard bb, Square from, tracked_array<Move, MAX_NUM_MOVES>& moves, PieceType type) {
    while(bb) {
        Square to = pop_lsb(bb);
        moves.emplace_back(from, to, type);
    }
}