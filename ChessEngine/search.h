#ifndef SEARCH_H_
#define SEARCH_H_

#include "board.h"

#include <vector>
#include <mutex>

struct Search
{
    Move best_move; 
	int best_eval; // current best evaluation value.
    int depth;      
    int nodes;
    int duration;   //in milliseconds
    bool stop;
    bool opening_book;

	// This should be locked when accessing 
	// members of the struct when the search 
	// thread is active.
	std::mutex search_guard;
};

// This function will be running in a separate thread.
bool StartSearch(Search *search, Board *board);

int AlphaBeta(Search *search, Board *board, int alpha, int beta, int depth, bool maximizing_side);

// Terminates the search thread.
void TerminateSearch(Search *search, bool terminate);

#endif
