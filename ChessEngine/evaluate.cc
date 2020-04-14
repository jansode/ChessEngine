#include "evaluate.h"

#define TERM(score,white_piece,black_piece) \
(score * (board->pieces_[white_piece] - board->pieces_[black_piece]))

namespace evaluation
{

int evaluate(Board *board)
{
	//TODO Improve evaluation
	int evaluation = 0;

	int material_score =
		TERM(kPawnScore, WHITE_PAWNS, BLACK_PAWNS) +
		TERM(kKnightScore, WHITE_KNIGHTS, BLACK_KNIGHTS) +
		TERM(kBishopScore, WHITE_BISHOPS, BLACK_BISHOPS) +
		TERM(kRookScore, WHITE_ROOKS, BLACK_ROOKS) +
		TERM(kQueenScore, WHITE_QUEEN, BLACK_QUEEN);

	evaluation += material_score;

	return evaluation;
}

int evaluate_pawn_structure(const Board& board, Side side)
{
	return 0;	//TODO
}

}
