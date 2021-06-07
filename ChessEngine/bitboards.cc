#include "bitboards.h"
#include <iostream>
#include <cassert>
#include <cctype> // std::tolower


void print_bitboard(Bitboard board) 
{
    Bitboard curr_square = kBitboardMSB;
    for(int i=0;i<8;++i)    
    {
        for(int j=0;j<8;++j, curr_square>>=1)
            (board & curr_square)?std::cout<<'1':std::cout<<'0';
        std::cout<<'\n';
    }
}

Bitboard bb_from_square(Square square)
{
    // TODO Make this just a table lookup or something. No need to bitshift.
    assert((int)square < SQUARE_NONE);
    return kBitboardMSB >> (int)square;
}

Bitboard RankFromSquare(Square square)
{
    assert((int)square < SQUARE_NONE);
    Bitboard rank = kBitboardRank1;
    Bitboard square_bb = bb_from_square(square);
    while(!(square_bb & rank))
        rank <<= 8;

    return rank;
}

Bitboard FileFromSquare(Square square)
{
    assert((int)square < SQUARE_NONE);
    Bitboard file = kBitboardFileA;
    Bitboard square_bb = bb_from_square(square);
    while(!(square_bb & file))
        file >>= 1;

    return file;
}

File GetSquareFile(Square square)
{
    Bitboard square_bb = bb_from_square(square);
    assert(PopulationCount(square_bb) == 1);

    Bitboard file_bb = kBitboardFileA;
    for(int curr_file=FILE_A;curr_file<NUM_FILES;++curr_file,file_bb>>=1)
        if(file_bb & square_bb) return (File)curr_file;

    return FILE_NONE;
}

Rank GetSquareRank(Square square)
{
    Bitboard square_bb = bb_from_square(square);
    assert(PopulationCount(square_bb) == 1);

    Bitboard rank_bb = kBitboardRank1;
    for(int curr_rank=RANK_1;curr_rank<NUM_RANKS;++curr_rank,rank_bb<<=8)
        if(rank_bb & square_bb) return (Rank)curr_rank;

    return RANK_NONE;
}

Bitboard BitboardFromAlgebraic(const std::string& algebraic) 
{
    assert(algebraic.size() == 2);

    char file = std::tolower(algebraic[0]);
    char rank = algebraic[1];

    Bitboard file_bb = 0ULL, rank_bb = 0ULL;
    switch(file)
    {
        case 'a': file_bb = kBitboardFileA; break; 
        case 'b': file_bb = kBitboardFileB; break;
        case 'c': file_bb = kBitboardFileC; break;
        case 'd': file_bb = kBitboardFileD; break;
        case 'e': file_bb = kBitboardFileE; break;
        case 'f': file_bb = kBitboardFileF; break;
        case 'g': file_bb = kBitboardFileG; break;
        case 'h': file_bb = kBitboardFileH; break;
    }

    switch(rank)
    {
        case '1': rank_bb = kBitboardRank1; break;
        case '2': rank_bb = kBitboardRank2; break;
        case '3': rank_bb = kBitboardRank3; break;
        case '4': rank_bb = kBitboardRank4; break;
        case '5': rank_bb = kBitboardRank5; break;
        case '6': rank_bb = kBitboardRank6; break;
        case '7': rank_bb = kBitboardRank7; break;
        case '8': rank_bb = kBitboardRank8; break;
    }

    return rank_bb & file_bb;
}

Square square_from_bitboard(Bitboard board)
{
	if (board == 0ULL) return SQUARE_NONE;
	
    int square = 0;
    Bitboard curr_square_bb = kBitboardMSB;
    while(!(curr_square_bb & board))
    {
        curr_square_bb>>=1;
        ++square;
    }

    return (Square) square;
}

int PopulationCount(Bitboard board)
{
    int count = 0;
    while(board)
    {
        ++count;
        board &= (board-1);
    }
    return count;
}

Bitboard GetEdgeBitboard(Direction direction)
{
    assert(direction >= NORTH && direction < NUM_DIRECTIONS);
    switch(direction)
    {
        case NORTH:
            return kBitboardRank8;
        case NORTH_WEST:
            return kBitboardRank8|kBitboardFileA;
        case NORTH_EAST:
            return kBitboardRank8|kBitboardFileH;
        case SOUTH:
            return kBitboardRank1;
        case SOUTH_WEST:
            return kBitboardRank1|kBitboardFileA;
        case SOUTH_EAST:
            return kBitboardRank1|kBitboardFileH;
        case EAST:
            return kBitboardFileH;
        case WEST:
            return kBitboardFileA;
    }

    return 0ULL;
}

template <Direction direction>
int squares_to_the_edge(Bitboard square)
{
    assert(PopulationCount(square) == 1);
    Bitboard edge = GetEdgeBitboard(direction); 
    int num_squares = 0;
    for(;!(square & edge);++num_squares)
    {
        square = ShiftDirection(square,direction,1);
    }
    return num_squares;
}

int squares_to_the_edge(Bitboard square ,Direction direction)
{
    assert(PopulationCount(square) == 1);
    Bitboard edge = GetEdgeBitboard(direction); 
    int num_squares = 0;
    for(;!(square & edge);++num_squares)
    {
        square = ShiftDirection(square,direction,1);
    }
    return num_squares;
}

Bitboard ShiftDirection(Bitboard board, Direction direction, int shift_amount)
{
    assert(shift_amount >= 0);

    switch(direction)
    {
        case NORTH:         return board << (8*shift_amount);
        case NORTH_EAST:    return board << (7*shift_amount);
        case NORTH_WEST:    return board << (9*shift_amount);
        case SOUTH_WEST:    return board >> (7*shift_amount);
        case SOUTH_EAST:    return board >> (9*shift_amount);
        case SOUTH:         return board >> (8*shift_amount);
        case WEST:          return board << shift_amount;
        case EAST:          return board >> shift_amount;
    }

    return 0ULL;
}

Bitboard BlockerRay(Bitboard square_bb, Direction direction, Bitboard blockers)
{
    assert(PopulationCount(square_bb) == 1);

    Bitboard edge = GetEdgeBitboard(direction);
    Bitboard result = 0ULL;
    if(edge & square_bb) return result;

    Bitboard curr_square = ShiftDirection(square_bb,direction,1);
    for(;!(curr_square&blockers) && !(curr_square & edge);curr_square=ShiftDirection(curr_square,direction,1))
    {
        result |= curr_square;
    }

    result |= curr_square;

    return result;
}

Square PopLSB(Bitboard *board)
{
    Bitboard square = *board & ~(*board-1);
    *board &= *board-1;
    return square_from_bitboard(square);
}

