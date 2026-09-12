#include "magic_bitboard.hpp"
#include "utility/util.hpp"


// max number of relevant occupancy bits for rook and bishop occupancy bitboards
// are 4096 and 512 respectively, hence they are the size of the arrays
std::array<Bitboard, 4096> ROOK_ATTACKS[64] = {};
std::array<Bitboard, 512>  BISHOP_ATTACKS[64] = {};

void init_occupancy_masks();
void init_attack_tables();

void init_magic_bbs() {
    init_occupancy_masks();
    init_attack_tables();
}

// ----------------------------------------------------------------------------
// Occupancy Masks
// ----------------------------------------------------------------------------

Bitboard ROOK_OCCUPANCY_MASK[64] = {};
Bitboard BISHOP_OCCUPANCY_MASK[64] = {};

Bitboard rook_mask(Square sq);
Bitboard bishop_mask(Square sq);

void init_occupancy_masks() {

    for (Square sq = A1; sq <= H8; sq++) {
        ROOK_OCCUPANCY_MASK[sq] |= rook_mask(sq);
        BISHOP_OCCUPANCY_MASK[sq] |= bishop_mask(sq);
    }
}

Bitboard rook_mask(Square sq) {
    Bitboard mask = 0;
    int r = sq / 8, f = sq % 8;

    for (int rr = r + 1; rr <= 6; rr++) mask |= 1ULL << (rr * 8 + f);
    for (int rr = r - 1; rr >= 1; rr--) mask |= 1ULL << (rr * 8 + f);
    for (int ff = f + 1; ff <= 6; ff++) mask |= 1ULL << (r * 8 + ff);
    for (int ff = f - 1; ff >= 1; ff--) mask |= 1ULL << (r * 8 + ff);

    return mask;
}

Bitboard bishop_mask(Square sq) {
    Bitboard mask = 0;
    int r = sq / 8, f = sq % 8;

    for (int rr = r+1, ff = f+1; rr <= 6 && ff <= 6; rr++, ff++) mask |= 1ULL << (rr*8+ff);
    for (int rr = r+1, ff = f-1; rr <= 6 && ff >= 1; rr++, ff--) mask |= 1ULL << (rr*8+ff);
    for (int rr = r-1, ff = f+1; rr >= 1 && ff <= 6; rr--, ff++) mask |= 1ULL << (rr*8+ff);
    for (int rr = r-1, ff = f-1; rr >= 1 && ff >= 1; rr--, ff--) mask |= 1ULL << (rr*8+ff);

    return mask;
}

// ----------------------------------------------------------------------------
// Populating Attack Tables
// ----------------------------------------------------------------------------

std::vector<Bitboard> get_mask_subsets(Bitboard mask);

void init_attack_tables() {
    for (Square sq = A1; sq <= H8; sq++) {
        for (Bitboard subset : get_mask_subsets(ROOK_OCCUPANCY_MASK[sq])) {
            Bitboard idx = (subset * ROOK_MAGIC[sq]) >> ROOK_SHIFT[sq];
            ROOK_ATTACKS[sq][idx] = rook_attacks_from_blockers(sq, subset);
        }

        for (Bitboard subset : get_mask_subsets(BISHOP_OCCUPANCY_MASK[sq])) {
            Bitboard idx = (subset * BISHOP_MAGIC[sq]) >> BISHOP_SHIFT[sq];
            BISHOP_ATTACKS[sq][idx] = bishop_attacks_from_blockers(sq, subset);
        }
    }
}

// Carry-Rippler subset traversal: https://chessprogramming.org/Traversing_Subsets_of_a_Set
std::vector<Bitboard> get_mask_subsets(Bitboard mask) {
    std::vector<Bitboard> subsets;
    Bitboard subset = 0;
    do {
        subsets.push_back(subset);
        subset = (subset - mask) & mask;
    } while (subset);
    return subsets;
}

Bitboard rook_attacks_from_blockers(Square sq, Bitboard blockers) {
    Bitboard attacks = 0;
    int r = sq / 8, f = sq % 8;

    for (int rr = r+1; rr <= 7; rr++) { Bitboard b = 1ULL << (rr*8+f); attacks |= b; if (b & blockers) break; }
    for (int rr = r-1; rr >= 0; rr--) { Bitboard b = 1ULL << (rr*8+f); attacks |= b; if (b & blockers) break; }
    for (int ff = f+1; ff <= 7; ff++) { Bitboard b = 1ULL << (r*8+ff); attacks |= b; if (b & blockers) break; }
    for (int ff = f-1; ff >= 0; ff--) { Bitboard b = 1ULL << (r*8+ff); attacks |= b; if (b & blockers) break; }

    return attacks;
}

Bitboard bishop_attacks_from_blockers(Square sq, Bitboard blockers) {
    Bitboard attacks = 0;
    int r = sq / 8, f = sq % 8;

    for (int rr=r+1, ff=f+1; rr<=7 && ff<=7; rr++, ff++) { Bitboard b = 1ULL<<(rr*8+ff); attacks |= b; if (b & blockers) break; }
    for (int rr=r+1, ff=f-1; rr<=7 && ff>=0; rr++, ff--) { Bitboard b = 1ULL<<(rr*8+ff); attacks |= b; if (b & blockers) break; }
    for (int rr=r-1, ff=f+1; rr>=0 && ff<=7; rr--, ff++) { Bitboard b = 1ULL<<(rr*8+ff); attacks |= b; if (b & blockers) break; }
    for (int rr=r-1, ff=f-1; rr>=0 && ff>=0; rr--, ff--) { Bitboard b = 1ULL<<(rr*8+ff); attacks |= b; if (b & blockers) break; }

    return attacks;
}