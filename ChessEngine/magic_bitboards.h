#ifndef MAGIC_BITBOARDS_H_
#define MAGIC_BITBOARDS_H_

#include "bitboards.h"

namespace magic_bitboards
{

void init();

// Used to calculate sliding piece attacks.
// @occupied: Bitboard of occupied pieces.

Bitboard rook_moves(Bitboard occupied, Square square);
Bitboard bishop_moves(Bitboard occupied, Square square);
Bitboard queen_moves(Bitboard occupied, Square square);

}

#endif
