#ifndef ATTACKS_H_
#define ATTACKS_H_

#include "types.h"
#include "bitboards.h"

enum PieceAttacks
{
    ATTACKS_WHITE_PAWN,
    ATTACKS_BLACK_PAWN,
    ATTACKS_KNIGHT,
    ATTACKS_BISHOP,
    ATTACKS_ROOK,
    ATTACKS_QUEEN,
    ATTACKS_KING,
    NUM_ATTACKS,
    ATTACKS_NONE
};

// Lookup table for piece attacks.
//
// Note: The edge squares don't need to
// be included for sliding pieces.
// 
// Table size: 7 * 64 * 8bytes = 3584bytes.
extern Bitboard attacks[NUM_ATTACKS][NUM_SQUARES];

void init_attacks();

// Returns the index into the attacks table 
// corresponding to the given piece type.
int piece_type_to_attacks_index(PieceType piece);

#endif // ATTACKS_H_
