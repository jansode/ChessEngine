#include "search.h"
#include "evaluate.h"
#include "move_generation.h"
#include "transposition.h"
#include "types.h"
#include "util.h"

#include <iostream>
#include <algorithm> // for std::min and std::max
#include <cmath>
#include <limits>

//TranspositionTable transposition_table;

void TerminateSearch(Search *search, bool terminate)
{
	search->search_guard.lock();
    search->stop = terminate; 
	search->search_guard.unlock();
}

bool StartSearch(Search *search, Board *board)
{
	std::cout<<"Search started\n";
    TerminateSearch(search, false);

	search->best_move = NULL_MOVE;
	search->best_eval = evaluation::kDrawScore;

	AlphaBeta(search,board,std::numeric_limits<int>::min(),std::numeric_limits<int>::max(),3,true);

	std::cout<<"Search stopped\n";

	std::cout<<"Best move: "<<move_to_algebraic(search->best_move)<<"\n";
	return true;
}

// A naive alpha beta search just to have something that works. 
int AlphaBeta(Search *search, Board *board, int alpha, int beta, int depth, bool maximizing_side)
{
	std::vector<Move> moves; 

    if(board->SideToMove() == WHITE)
	    move_generation::LegalAll<WHITE>(*board,&moves);
    else
	    move_generation::LegalAll<BLACK>(*board,&moves);

	if(depth == 0 || moves.size() == 0)	
		return evaluation::evaluate(board);

	if(maximizing_side)
	{
		int value = std::numeric_limits<int>::max();
		for(int i=0;i<moves.size();++i)
		{
			board->MakeMove(moves[i]);
			value = std::max(value,AlphaBeta(search,board,alpha,beta,depth-1,false));
			if(value > search->best_eval)
			{
				search->best_eval = value;
				search->best_move = moves[i];
			}

			board->UndoMove();

			alpha = std::max(alpha,value);

			if(alpha >= beta) break;

			return value;
		}
	}
	else
	{
		int value = std::numeric_limits<int>::min();		
		for(int i=0;i<moves.size();++i)
		{
			board->MakeMove(moves[i]);
			value = std::min(value,AlphaBeta(search,board,alpha,beta,depth-1,true));
			board->UndoMove();

			beta = std::min(beta,value);

			if(alpha >= beta) break;

			return value;
		}
	}

    return 0;
}



