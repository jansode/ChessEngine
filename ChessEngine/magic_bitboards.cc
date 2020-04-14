#include "magic_bitboards.h"
#include "attacks.h"

#include <iostream>
#include <cassert>
#include <random>

// All these tables are calculated at program startup.
// We could just calculate the magic numbers once and
// hardcode the values in the engine. I think I'll do 
// that at some point. 
// Calculating all this at the beginning slows down 
// the program startup considerably.

// TODO find out why I've commented the shifts.

namespace
{

int rook_shifts[NUM_SQUARES] = { 0 };
int bishop_shifts[NUM_SQUARES] = { 0 };

std::random_device rd;
std::mt19937_64 eng(rd());

std::uniform_int_distribution<unsigned long long> dist;

u64 rook_magic_numbers[NUM_SQUARES] = { 0 };
u64 bishop_magic_numbers[NUM_SQUARES] = { 0 };

std::vector<Bitboard> rook_magic_moves[NUM_SQUARES];
std::vector<Bitboard> bishop_magic_moves[NUM_SQUARES];

std::vector<Bitboard> rook_occupancy_variations[NUM_SQUARES];
std::vector<Bitboard> bishop_occupancy_variations[NUM_SQUARES];

// Gets all the possible combinations of bits on a particular ray
// from the given square to the edge of the board. 
std::vector<Bitboard> ray_combinations(Bitboard square, Direction direction)
{
	std::vector<Bitboard> combinations;

	int num_squares = squares_to_the_edge(square, direction);

	// To get all the combinations 
	// of bits we count up to 
	// the nth power of two where
	// n is the number of bits. 
	// We then extract these 
	// bits and shift them in
	// the direction of the ray.

	Bitboard current;
	u8 target = 1 << num_squares;

	for (u8 counter = 0; counter < target; ++counter)
	{
		current = 0ULL;
		for (u8 bit = 0; bit < num_squares; ++bit)
		{
			if (counter & (1ULL << bit))
			{
				current |= ShiftDirection(square, direction, bit);
			}
		}

		combinations.push_back(current);
	}
	return combinations;
}

void init_rook_occupancy_variations()
{
	int directions[4] = { NORTH,EAST,WEST,SOUTH };

	std::vector<std::vector<Bitboard>> combinations;

	for (int square = A8;square < NUM_SQUARES;++square)
	{
		Bitboard square_bb = bb_from_square((Square)square);

		// Generate all the bit combinations from the current 
		// square to each direction. The start square is 
		// not included.

		for (int direction = 0;direction < 4;++direction)
		{
			if (squares_to_the_edge(square_bb, (Direction)directions[direction]) == 0)
			{
				// We need to insert this to be able to
				// combine the directions later on. 
				combinations.push_back(std::vector<Bitboard>({ 0ULL }));
				continue;
			}

			Bitboard start_square_bb = ShiftDirection(square_bb, (Direction)directions[direction], 1);

			combinations.push_back(ray_combinations(start_square_bb, (Direction)directions[direction]));
		}

		// Combine each of the direction variations with
		// all the others and add them to the final 
		// variations table.
		std::vector<Bitboard> tmp;
		for (int i = 0;i < combinations[0].size();++i)
		{
			for (int j = 0;j < combinations[1].size();++j)
			{
				for (int k = 0;k < combinations[2].size();++k)
				{
					for (int l = 0;l < combinations[3].size();++l)
					{
						tmp.push_back(combinations[0][i] |
							combinations[1][j] |
							combinations[2][k] |
							combinations[3][l]);
					}
				}
			}
		}

		rook_occupancy_variations[square].insert(
			rook_occupancy_variations[square].end(),
			tmp.begin(),
			tmp.end());

		combinations.clear();
	}
}

void init_bishop_occupancy_variations()
{
	int directions[4] = { NORTH_WEST,NORTH_EAST,SOUTH_WEST,SOUTH_EAST };

	std::vector<std::vector<Bitboard>> combinations;

	for (int square = A8;square < NUM_SQUARES;++square)
	{
		Bitboard square_bb = bb_from_square((Square)square);

		// Generate all the bit combinations from the current 
		// square to each direction. The start square is 
		// not included.
		for (int direction = 0;direction < 4;++direction)
		{
			if (squares_to_the_edge(square_bb, (Direction)directions[direction]) == 0)
			{
				// We need to insert this to be able to
				// combine the directions later on. 
				combinations.push_back(std::vector<Bitboard>({ 0ULL }));
				continue;
			}

			Bitboard start_square_bb = ShiftDirection(square_bb, (Direction)directions[direction], 1);

			combinations.push_back(ray_combinations(start_square_bb, (Direction)directions[direction]));
		}

		assert(combinations[0].size() != 0);
		assert(combinations[1].size() != 0);
		assert(combinations[2].size() != 0);
		assert(combinations[3].size() != 0);

		// Combine each of the direction variations with
		// all the others and add them to the final 
		// variations table.
		std::vector<Bitboard> tmp;
		for (int i = 0;i < combinations[0].size();++i)
		{
			for (int j = 0;j < combinations[1].size();++j)
			{
				for (int k = 0;k < combinations[2].size();++k)
				{
					for (int l = 0;l < combinations[3].size();++l)
					{
						tmp.push_back(combinations[0][i] |
							combinations[1][j] |
							combinations[2][k] |
							combinations[3][l]);
					}
				}
			}
		}

		bishop_occupancy_variations[square].insert(
			bishop_occupancy_variations[square].end(),
			tmp.begin(),
			tmp.end());

		combinations.clear();
	}
}

u64 Random64() { return dist(eng); }

void init_shifts()
{
	for (int i = 0;i < NUM_SQUARES;++i)
	{
		rook_shifts[i] = PopulationCount(attacks[ATTACKS_ROOK][i]);
		bishop_shifts[i] = PopulationCount(attacks[ATTACKS_BISHOP][i]);
	}
}

void generate_rook_magics()
{
	for (int square = 0;square < NUM_SQUARES;++square)
	{
		Bitboard square_bb = bb_from_square((Square)square);
		rook_magic_moves[square].assign(1ULL << 14, 0ULL);//(rook_shifts[square]),~0ULL);

		bool magic_found = false;
		while (!magic_found)
		{
			std::fill(rook_magic_moves[square].begin(),
				rook_magic_moves[square].end(),
				0ULL);

			u64 rand64 = Random64();
			for (int j = 0;j < rook_occupancy_variations[square].size();++j)
			{
				Bitboard variation = rook_occupancy_variations[square][j];
				Bitboard moves_board =
					BlockerRay(square_bb, NORTH, variation) |
					BlockerRay(square_bb, SOUTH, variation) |
					BlockerRay(square_bb, EAST, variation) |
					BlockerRay(square_bb, WEST, variation);

				u64 index = (variation * rand64) >> (64 - 14);//(rook_shifts[square]));

				if ((rook_magic_moves[square][index] == 0ULL)
					|| (rook_magic_moves[square][index] == moves_board))
				{
					rook_magic_moves[square][index] = moves_board;
					magic_found = true;
				}
				else
				{
					magic_found = false;
					break;
				}
			}
			rook_magic_numbers[square] = rand64;
		}
	}
}

void generate_bishop_magics()
{
	for (int square = 0;square < NUM_SQUARES;++square)
	{
		Bitboard square_bb = bb_from_square((Square)square);
		bishop_magic_moves[square].assign(1ULL << 12, 0ULL);//(rook_shifts[square]),~0ULL);

		bool magic_found = false;
		while (!magic_found)
		{
			std::fill(bishop_magic_moves[square].begin(),
				bishop_magic_moves[square].end(),
				0ULL);

			u64 rand64 = Random64();
			for (int j = 0;j < bishop_occupancy_variations[square].size();++j)
			{
				Bitboard variation = bishop_occupancy_variations[square][j];
				Bitboard moves_board =
					BlockerRay(square_bb, NORTH_EAST, variation) |
					BlockerRay(square_bb, NORTH_WEST, variation) |
					BlockerRay(square_bb, SOUTH_EAST, variation) |
					BlockerRay(square_bb, SOUTH_WEST, variation);

				u64 index = (variation * rand64) >> (64 - 12);//(rook_shifts[square]));

				if ((bishop_magic_moves[square][index] == 0ULL)
					|| (bishop_magic_moves[square][index] == moves_board))
				{
					bishop_magic_moves[square][index] = moves_board;
					magic_found = true;
				}
				else
				{
					magic_found = false;
					break;
				}
			}
			bishop_magic_numbers[square] = rand64;
		}
	}
}

}

namespace magic_bitboards
{


Bitboard rook_moves(Bitboard occupied, Square square)
{
	u64 index = (attacks[ATTACKS_ROOK][square] & occupied) * rook_magic_numbers[square] >> (64 - 14);
	return rook_magic_moves[square][index];
}

Bitboard bishop_moves(Bitboard occupied, Square square)
{
	u64 index = (attacks[ATTACKS_BISHOP][square] & occupied) * bishop_magic_numbers[square] >> (64 - 12);
	return bishop_magic_moves[square][index];
}

Bitboard queen_moves(Bitboard occupied, Square square)
{
	return rook_moves(occupied, square) | bishop_moves(occupied, square);
}

void init()
{
	std::cout << "Initializing magic bitboards...\n";
	init_rook_occupancy_variations();
	init_bishop_occupancy_variations();
	generate_rook_magics();
	generate_bishop_magics();
	init_shifts();
}

}

