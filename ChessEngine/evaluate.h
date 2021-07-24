#ifndef EVALUATE_H
#define EVALUATE_H

#include "board.h"

namespace evaluation
{

const int kQueenScore = 900;
const int kRookScore = 500;
const int kBishopScore = 300;
const int kKnightScore = 300;
const int kPawnScore = 100;

const int kDrawScore = 0;
const int kMateScore = 100000;

const int pawn_structure_weight = 1;

float evaluate(Board *board);

}

#endif
