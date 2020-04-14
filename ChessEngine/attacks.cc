#include "attacks.h"
#include <iostream>

Bitboard attacks[NUM_ATTACKS][NUM_SQUARES] = {0};

namespace
{

void init_pawn_attacks()
{
	std::cout << "Initializing pawn attacks...\n";
	for (Bitboard square_bb = kBitboardMSB;square_bb > 0;square_bb >>= 1)
	{
		int square = square_from_bitboard(square_bb);

		// Add white pawn attacks
		if ((square_bb & kBitboardRank8) == 0)
		{
			// Add up-left attacks
			if ((square_bb & kBitboardFileA) == 0)
				attacks[ATTACKS_WHITE_PAWN][square] |= (square_bb << 9);

			// Add up-right attacks
			if ((square_bb & kBitboardFileH) == 0)
				attacks[ATTACKS_WHITE_PAWN][square] |= (square_bb << 7);
		}

		//Add black pawn attacks
		if ((square_bb & kBitboardRank1) == 0)
		{
			// Add down-left attacks
			if ((square_bb & kBitboardFileA) == 0)
				attacks[ATTACKS_BLACK_PAWN][square] |= (square_bb >> 7);

			// Add down-right attacks
			if ((square_bb & kBitboardFileH) == 0)
				attacks[ATTACKS_BLACK_PAWN][square] |= (square_bb >> 9);
		}
	}
}

void init_knight_attacks()
{
	std::cout << "Initializing knight attacks...\n";
	for (Bitboard square_bb = kBitboardMSB;square_bb > 0;square_bb >>= 1)
	{
		int square = square_from_bitboard(square_bb);

		//Add left-left moves.
		if ((square_bb & (kBitboardFileA | kBitboardFileB)) == 0)
		{
			//Add left-left-up move.
			if ((square_bb & kBitboardRank8) == 0)
			{
				attacks[ATTACKS_KNIGHT][square] |= (square_bb << 10);
			}
			//Add left-left-down move.
			if ((square_bb & kBitboardRank1) == 0)
			{
				attacks[ATTACKS_KNIGHT][square] |= (square_bb >> 6);
			}
		}
		//Add right-right moves.
		if ((square_bb & (kBitboardFileG | kBitboardFileH)) == 0)
		{
			//Add right-right-up move.
			if ((square_bb & kBitboardRank8) == 0)
			{
				attacks[ATTACKS_KNIGHT][square] |= (square_bb << 6);
			}
			//Add right-right-down move.
			if ((square_bb & kBitboardRank1) == 0)
			{
				attacks[ATTACKS_KNIGHT][square] |= (square_bb >> 10);
			}
		}
		//Add down-down moves.
		if ((square_bb & (kBitboardRank1 | kBitboardRank2)) == 0)
		{
			//Add down-down-left move.
			if ((square_bb & kBitboardFileA) == 0)
			{
				attacks[ATTACKS_KNIGHT][square] |= (square_bb >> 15);
			}
			//Add down-down-right move.
			if ((square_bb & kBitboardFileH) == 0)
			{
				attacks[ATTACKS_KNIGHT][square] |= (square_bb >> 17);
			}
		}

		//Add up-up moves.
		if ((square_bb & (kBitboardRank7 | kBitboardRank8)) == 0)
		{
			//Add up-up-left move.
			if ((square_bb & kBitboardFileA) == 0)
			{
				attacks[ATTACKS_KNIGHT][square] |= (square_bb << 17);
			}
			//Add up-up-right move.
			if ((square_bb & kBitboardFileH) == 0)
			{
				attacks[ATTACKS_KNIGHT][square] |= (square_bb << 15);
			}
		}
	}
}

void init_bishop_attacks()
{
	std::cout << "Initializing bishop attacks...\n";
	for (Bitboard square_bb = kBitboardMSB;square_bb > 0;square_bb >>= 1)
	{
		int square = square_from_bitboard(square_bb);
		if ((square_bb & (kBitboardRank8 | kBitboardFileA)) == 0)
		{
			//Add north-west attacks
			Bitboard add_square_bb = square_bb << 9;
			while ((add_square_bb & (kBitboardRank8 | kBitboardFileA)) == 0)
			{
				attacks[ATTACKS_BISHOP][square] |= (add_square_bb);
				add_square_bb <<= 9;
			}
		}

		if ((square_bb & (kBitboardRank8 | kBitboardFileH)) == 0)
		{
			//Add north-east attacks
			Bitboard add_square_bb = square_bb << 7;
			while ((add_square_bb & (kBitboardRank8 | kBitboardFileH)) == 0)
			{
				attacks[ATTACKS_BISHOP][square] |= (add_square_bb);
				add_square_bb <<= 7;
			}
		}

		if ((square_bb & (kBitboardRank1 | kBitboardFileA)) == 0)
		{
			//Add south-west attacks
			Bitboard add_square_bb = square_bb >> 7;
			while ((add_square_bb & (kBitboardRank1 | kBitboardFileA)) == 0)
			{
				attacks[ATTACKS_BISHOP][square] |= (add_square_bb);
				add_square_bb >>= 7;
			}
		}

		if ((square_bb & (kBitboardRank1 | kBitboardFileH)) == 0)
		{
			//Add south-east attacks
			Bitboard add_square_bb = square_bb >> 9;
			while ((add_square_bb & (kBitboardRank1 | kBitboardFileH)) == 0)
			{
				attacks[ATTACKS_BISHOP][square] |= (add_square_bb);
				add_square_bb >>= 9;
			}
		}
	}
}

void init_rook_attacks()
{
	std::cout << "Initializing rook attacks...\n";
	for (Bitboard square_bb = kBitboardMSB;square_bb > 0;square_bb >>= 1)
	{
		int square = square_from_bitboard(square_bb);
		if ((square_bb & (kBitboardRank8)) == 0)
		{
			//Add north attacks
			Bitboard add_square_bb = square_bb << 8;
			while ((add_square_bb & (kBitboardRank8)) == 0)
			{
				attacks[ATTACKS_ROOK][square] |= (add_square_bb);
				add_square_bb <<= 8;
			}
		}

		if ((square_bb & (kBitboardRank1)) == 0)
		{
			//Add south attacks
			Bitboard add_square_bb = square_bb >> 8;
			while ((add_square_bb & (kBitboardRank1)) == 0)
			{
				attacks[ATTACKS_ROOK][square] |= (add_square_bb);
				add_square_bb >>= 8;
			}
		}

		if ((square_bb & (kBitboardFileA)) == 0)
		{
			//Add west attacks
			Bitboard add_square_bb = square_bb << 1;
			while ((add_square_bb & (kBitboardFileA)) == 0)
			{
				attacks[ATTACKS_ROOK][square] |= (add_square_bb);
				add_square_bb <<= 1;
			}
		}

		if ((square_bb & (kBitboardFileH)) == 0)
		{
			//Add east attacks
			Bitboard add_square_bb = square_bb >> 1;
			while ((add_square_bb & (kBitboardFileH)) == 0)
			{
				attacks[ATTACKS_ROOK][square] |= (add_square_bb);
				add_square_bb >>= 1;
			}
		}
	}
}

void init_queen_attacks()
{
	std::cout << "Initializing queen attacks...\n";
	for (Bitboard square_bb = kBitboardMSB;square_bb > 0;square_bb >>= 1)
	{
		int square = square_from_bitboard(square_bb);

		// The queen attacks is just the combination of
		// bishop and rook attacks.
		attacks[ATTACKS_QUEEN][square] = (attacks[ATTACKS_BISHOP][square] | attacks[ATTACKS_ROOK][square]);
	}
}

void init_king_attacks()
{
	std::cout << "Initializing king attacks...\n";
	for (Bitboard square_bb = kBitboardMSB;square_bb > 0;square_bb >>= 1)
	{
		int square = square_from_bitboard(square_bb);

		// Add up attacks.
		if ((square_bb & kBitboardRank8) == 0)
		{
			attacks[ATTACKS_KING][square] |= square_bb << 8;

			// Add up-right attack.
			if ((square_bb & kBitboardFileA) == 0)
				attacks[ATTACKS_KING][square] |= square_bb << 9;

			// Add up-left attack.
			if ((square_bb & kBitboardFileH) == 0)
				attacks[ATTACKS_KING][square] |= square_bb << 7;
		}

		// Add down attacks.
		if ((square_bb & kBitboardRank1) == 0)
		{
			attacks[ATTACKS_KING][square] |= square_bb >> 8;

			// Add down-right attack.
			if ((square_bb & kBitboardFileA) == 0)
				attacks[ATTACKS_KING][square] |= square_bb >> 7;

			// Add down-left attack.
			if ((square_bb & kBitboardFileH) == 0)
				attacks[ATTACKS_KING][square] |= square_bb >> 9;
		}

		// Add left attack.
		if ((square_bb & kBitboardFileA) == 0)
			attacks[ATTACKS_KING][square] |= square_bb << 1;

		// Add right attack.
		if ((square_bb & kBitboardFileH) == 0)
			attacks[ATTACKS_KING][square] |= square_bb >> 1;
	}
}

}

void init_attacks()
{
    init_pawn_attacks();
    init_knight_attacks();
    init_bishop_attacks();
    init_rook_attacks();
    init_queen_attacks();
    init_king_attacks();
}


int piece_type_to_attacks_index(PieceType piece)
{
    switch(piece)
    {
        case WHITE_PAWNS:
            return ATTACKS_WHITE_PAWN;
        case BLACK_PAWNS:
            return ATTACKS_BLACK_PAWN;
        case WHITE_KNIGHTS:
        case BLACK_KNIGHTS:
            return ATTACKS_KNIGHT;
        case WHITE_BISHOPS:
        case BLACK_BISHOPS:
            return ATTACKS_BISHOP;
        case WHITE_ROOKS:
        case BLACK_ROOKS:
            return ATTACKS_ROOK;
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            return ATTACKS_QUEEN;
        case WHITE_KING:
        case BLACK_KING:
            return ATTACKS_KING;
        default:
            return ATTACKS_NONE;
    }
}
