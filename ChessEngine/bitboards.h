#ifndef BITBOARD_H_
#define BITBOARD_H_

#include "types.h"

#include <string>
#include <vector>

// Utility functions and constants for working with bitboards.

const Bitboard kBitboardMSB = 1ULL << 63;
const Bitboard kBitboardRank1 = 0xFF;
const Bitboard kBitboardRank2 = kBitboardRank1 << 8;
const Bitboard kBitboardRank3 = kBitboardRank2 << 8;
const Bitboard kBitboardRank4 = kBitboardRank3 << 8;
const Bitboard kBitboardRank5 = kBitboardRank4 << 8;
const Bitboard kBitboardRank6 = kBitboardRank5 << 8;
const Bitboard kBitboardRank7 = kBitboardRank6 << 8;
const Bitboard kBitboardRank8 = kBitboardRank7 << 8;

const Bitboard kBitboardFileA = 0x8080808080808080;
const Bitboard kBitboardFileB = kBitboardFileA >> 1;
const Bitboard kBitboardFileC = kBitboardFileB >> 1;
const Bitboard kBitboardFileD = kBitboardFileC >> 1;
const Bitboard kBitboardFileE = kBitboardFileD >> 1;
const Bitboard kBitboardFileF = kBitboardFileE >> 1;
const Bitboard kBitboardFileG = kBitboardFileF >> 1;
const Bitboard kBitboardFileH = kBitboardFileG >> 1;

const Bitboard kBitboardEdges = kBitboardRank1|kBitboardRank8|kBitboardFileA|kBitboardFileH;

// The squares between the rook and the king.
const Bitboard kCastlingSquaresWK = 0x2ULL | 4;
const Bitboard kCastlingSquaresWQ = (kCastlingSquaresWK << 4) | 0x10;
const Bitboard kCastlingSquaresBK = kCastlingSquaresWK << 7*8;
const Bitboard kCastlingSquaresBQ = kCastlingSquaresWQ << 7*8;

extern Bitboard kSquareBitboard[NUM_SQUARES]; 

void init_bitboards();

// Prints a binary representation of the bitboard to stdout. 
void print_bitboard(Bitboard board);

// Returns a bitboard with the given square set. 
Bitboard bb_from_square(Square square);

// Returns a bitboard where the rank of the given square is set. 
Bitboard RankFromSquare(Bitboard square);

// Returns a bitboard where the file of the given square is set. 
Bitboard FileFromSquare(Bitboard square);

// Returns a bitboard where the file of the given square is set. 
Bitboard GetSquareFile(Bitboard square);

// Returns the file enumeration of the file.
File GetSquareFile(Square square);

// Returns the rank enumeration of the rank.
Rank GetSquareRank(Square square);

// Returns a bitboard where the given square(in algebraic notation) is set. 
Bitboard BitboardFromAlgebraic (const std::string& algebraic);

// Returns a square enumeration from the given bitboard square. 
Square square_from_bitboard(Bitboard board);

// Returns the number of set bits on the bitboard.
int PopulationCount(Bitboard board);

// Returns a bitboard that is shifted shift_amount times
// in the specified direction from the given square.
Bitboard ShiftDirection(Bitboard board, Direction direction, int shift_amount);

// Returns a bitboard where the edge squares are set in 
// the given direction. 
Bitboard GetEdgeBitboard(Direction direction);

// Returns the number of squares from a square to
// the edge of the board in the given direction. The edge
// square is included in the result.
template <Direction direction>
int squares_to_the_edge(Bitboard square);

int squares_to_the_edge(Bitboard square, Direction direction);

// Returns a bitboard where the ray from the 
// square to the first blocker of the blocker
// bitboard is set. The start square is not 
// included in the result.
Bitboard BlockerRay(Bitboard square_bb, Direction direction, Bitboard blockers);

// Resets the least significant set bit 
// and returns the corresponding square.
Square PopLSB(Bitboard *board);

#endif //BITBOARD_H_
