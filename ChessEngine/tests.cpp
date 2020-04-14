#include <vector>
#include <iostream>
#include <fstream>

#include "attacks.h"
#include "tests.h"
#include "types.h"
#include "board.h"
#include "move_generation.h"
#include "util.h"
#include "bitboards.h"

namespace
{
	std::vector<PerftResults> PerftTestPositions;

	void read_perft_results_file(const std::string& test_positions_file)
	{
		std::ifstream file(test_positions_file);

		std::string line;
		while (getline(file, line))
		{
			std::string fen;
			PerftResults results;
			std::vector<std::string> split_line = split_string(line, ';');

			if (split_line.size() == 0)
			{
				std::cout << "Could not read perft results file: " 
					<< test_positions_file << "\n";
				return;
			}

			// TODO make validate fen function.
			results.fen = split_line[0];
			std::vector<PerftStats> stats;

			for (int i = 1; i < split_line.size(); ++i)
			{
				PerftStats new_stats;

				// Skip the depth value 
				std::string nodes_substr = "";
				auto pos = split_line[i].find(' ');
				nodes_substr = split_line[i].substr(pos);

				// TODO extend to read other stats values.
				new_stats.nodes = std::stoi(nodes_substr);
				stats.push_back(new_stats);
			}

			results.stats = stats;
			results.to_depth = split_line.size() - 1;
			
			PerftTestPositions.push_back(results);
		}
	}

	int perft(Board board, int depth, PerftStats *stats)
	{
		std::vector<Move> move_list;
		int nodes = 0;

		if (depth == 0) return 1;

		move_generation::LegalAll(board, &move_list, board.state_.side_to_move);
		for (int i = 0;i < move_list.size();++i)
		{
			board.MakeMove(move_list[i]);
			unsigned added_nodes = 0;
			added_nodes = perft(board, depth - 1, stats);
			nodes += added_nodes;

			if (move_list[i].capture) ++stats->captures;
			if (board.InCheck(board.state_.side_to_move))
			{
				++stats->checks;
				if (added_nodes == 0)
					++stats->checkmates;
			}

			switch (move_list[i].type)
			{
			case DOUBLE_PAWN: ++stats->double_pawn; break;
			case CASTLE_KINGSIDE:
			case CASTLE_QUEENSIDE: ++stats->castles; break;
			case PROMOTION: ++stats->promotions; break;
			default: break;
			}
			board.UndoMove();
		}
		return nodes;
	}
}

namespace tests
{
	void init_perft(const std::string& perft_results_file)
	{
		std::cout << "Initializing tests...\n";
		read_perft_results_file(perft_results_file);
	}

	// Automated perft tests that 
	// checks against the results 
	// in the PerftTestResults vector.
	void start_perft(unsigned depth_limit)
	{
		Board test_board;
		for (auto result : PerftTestPositions)
		{
			std::cout << "Fen: " << result.fen << "\n";
			test_board.SetPositionFromFEN(result.fen);
			test_board.PrintPosition();
			start_perft(test_board, (depth_limit < result.to_depth)?depth_limit:result.to_depth, &result);
		}
	}

	void start_perft(const Board& board, unsigned depth, const PerftResults* results)
	{
		++depth;

		unsigned nodes = 0;
		PerftStats stats;
		for (int curr_depth = 0; curr_depth < depth; ++curr_depth)
		{
			nodes = perft(board, curr_depth, &stats);

			if (curr_depth == 0) continue;

			if (results != nullptr)
			{
				// Currently checks only nodes.
				std::cout << "Depth: " << curr_depth;
				std::cout << " Nodes: " << nodes;
				std::cout << " Correct nodes: " << results->stats[curr_depth - 1].nodes;

				std::string depth_ok = (results->stats[curr_depth-1].nodes == nodes) ? " [OK]" : " [FAILED]";
				std::cout << depth_ok << "\n";
			}
			else
			{
				std::cout << "Depth: " << curr_depth
					<< " Nodes: " << nodes
					<< " Captures: " << stats.captures
					<< " Castles: " << stats.castles
					<< " Promotions: " << stats.promotions
					<< " Checks: " << stats.checks
					<< " Checkmates: " << stats.checkmates
					<< "\n";
			}
			nodes = 0;
			stats = { 0 };
		}
	}

	void print_attacks(PieceType piece, Square square)
	{
		if (square == SQUARE_NONE) return;

		int attacks_index = piece_type_to_attacks_index(piece);
		Bitboard piece_attacks = attacks[attacks_index][square];

		std::cout << "Square: " << algebraic_from_square(square) << "\n";
		print_bitboard(piece_attacks);
		std::cout << "\n";
	}

	void print_attacks(PieceType piece)
	{
		for (int curr_square = A8; curr_square < NUM_SQUARES; ++curr_square)
		{
			print_attacks(piece, (Square) curr_square);
		}
	}
}


