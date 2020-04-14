#ifndef MOVE_H_
#define MOVE_H_

#include "types.h"

#include <string>

class Board;

enum MoveType
{
    NORMAL,
    DOUBLE_PAWN,
    CASTLE_KINGSIDE,
    CASTLE_QUEENSIDE,
    PROMOTION,
    MOVE_TYPE_NONE
};

// Should change this to a constructor in the move struct instead.
#define NULL_MOVE {SQUARE_NONE,SQUARE_NONE,PIECE_TYPE_NONE,MOVE_TYPE_NONE,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false}

struct Move
{
    Square      from;
    Square      to;
    PieceType   piece;
    MoveType    type;
    PieceType   promotion;
    PieceType   captured_type;
    bool        capture;
};

// Creates a move structure from a UCI move string token.
Move move_from_uci(const Board& board, const std::string& move_str);

std::string get_move_type(MoveType movetype);

void print_move(const Move &m);

std::string move_to_algebraic(const Move &m);

#endif //MOVE_H_
