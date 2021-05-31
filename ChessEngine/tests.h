#pragma once

#include <vector>
#include <sstream>

#include "board.h"

// Source: http://www.rocechess.ch/perft.html
const std::string kPerftResultsFile = "perftsuite.epd";

const int kDefaultPerftDepth = 3;

struct PerftStats
{
	unsigned nodes = 0;
	unsigned depth = 0;
	unsigned captures = 0;
	unsigned promotions = 0;
	unsigned castles = 0;
	unsigned checks = 0;
	unsigned double_pawn = 0;
	unsigned checkmates = 0;
};

struct PerftResults
{
	// Fen string to compare results for.  
	std::string fen;

	// Stats for each depth ordered by index.
	std::vector<PerftStats> stats; 

	// The depth to terminate the test.
	unsigned to_depth;
};

namespace tests
{
	const unsigned DEPTH_LIMIT_UNLIMITED = UINT_MAX;

	void init_perft(const std::string& perft_results_file = kPerftResultsFile);

	// Starts automated perft test that checks the generated moves
	// against results in the perft results file.
	// @depth_limit(optional): Max depth of a single search.
	void start_perft(unsigned depth_limit = DEPTH_LIMIT_UNLIMITED, bool display_only_failed = false);

	// Starts perft test using the current board position as a starting point.
	// @results(optional): Pointer to PerftResults structure for results to check against.
    // Returns true if PerftResults matches with the engine results and false if any errors
    // were found.
	bool start_perft(const Board& board, unsigned depth, std::ostringstream& result_str, const PerftResults* results = nullptr);

	// Prints a visual bitboard representation of the attacks for 
	// the given piece for each square on the board. 
	// @piece: PieceType enum of the piece to print the attacks for.
	void print_attacks(PieceType piece);
}
