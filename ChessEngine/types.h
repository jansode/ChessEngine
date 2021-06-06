#ifndef TYPES_H_
#define TYPES_H_

#include <cstdint>
#include <climits>

using u8 = uint8_t;
using u64 = uint64_t;
using Bitboard = u64;

enum Square
{
    A8,B8,C8,D8,E8,F8,G8,H8,
    A7,B7,C7,D7,E7,F7,G7,H7,
    A6,B6,C6,D6,E6,F6,G6,H6,
    A5,B5,C5,D5,E5,F5,G5,H5,
    A4,B4,C4,D4,E4,F4,G4,H4,
    A3,B3,C3,D3,E3,F3,G3,H3,
    A2,B2,C2,D2,E2,F2,G2,H2,
    A1,B1,C1,D1,E1,F1,G1,H1,
    NUM_SQUARES,
    SQUARE_NONE
};

enum Direction
{
    NORTH,
    SOUTH,
    WEST,
    EAST,
    NORTH_WEST,
    NORTH_EAST,
    SOUTH_WEST,
    SOUTH_EAST,
    NUM_DIRECTIONS,
    DIRECTION_NONE
};

enum PieceType
{
    WHITE_PAWNS,
    WHITE_KNIGHTS,
    WHITE_BISHOPS,
    WHITE_ROOKS,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWNS,
    BLACK_KNIGHTS,
    BLACK_BISHOPS,
    BLACK_ROOKS,
    BLACK_QUEEN,
    BLACK_KING,
    NUM_PIECE_TYPES,
    PIECE_TYPE_NONE
};

enum Piece
{
    PAWNS,
    KNIGHTS,
    BISHOPS,
    ROOKS,
    QUEENS,
    KINGS,
    NUM_PIECES,
    PIECE_NONE
};

enum Rank
{
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    NUM_RANKS,
    RANK_NONE
};

enum File
{
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    NUM_FILES,
    FILE_NONE
};

enum Side
{
    WHITE,
    BLACK,
    NUM_SIDES,
    SIDE_NONE
};

enum Castling
{
    BLACK_QUEENSIDE = 0x1,
    BLACK_KINGSIDE = 0x2,
    WHITE_QUEENSIDE = 0x4,
    WHITE_KINGSIDE = 0x8
};


#endif // TYPES_H 
