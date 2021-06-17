#include "board.h"
#include "move.h"
#include "util.h"
#include "move_generation.h"
#include "bitboards.h"

#include <string>
#include <iostream>

Move move_from_uci(const Board& board, const std::string& move_str)
{
    Side side = board.state_.side_to_move;

    // If no move can be parsed, return these defaults.
    Move new_move = {SQUARE_NONE,
                     SQUARE_NONE,
                     PIECE_TYPE_NONE,
                     MOVE_TYPE_NONE,
                     PIECE_TYPE_NONE,
                     PIECE_TYPE_NONE,
                     false}; 

    if(move_str.size() < 4) return new_move;

    Square from = square_from_algebraic(std::string() + move_str[0] + move_str[1]);
    Square to = square_from_algebraic(std::string() + move_str[2] + move_str[3]);

	PieceType piece = board.GetPieceOnSquare(from);

    Board tmp_board = board; 

	// Check if king moves into a castling move.
	if (piece == WHITE_KING || piece == BLACK_KING)
	{
		if (to == G1 && from == E1 && board.CanCastle(WHITE, WHITE_KINGSIDE))
		{
			new_move.type = CASTLE_KINGSIDE;
            new_move.from = from;
            new_move.to = to;
			new_move.piece = WHITE_KING;
			return new_move;
		}
		else if (to == C1 && from == E1 && board.CanCastle(WHITE, WHITE_QUEENSIDE))
		{
			new_move.type = CASTLE_QUEENSIDE;
            new_move.from = from;
            new_move.to = to;
			new_move.piece = WHITE_KING;
			return new_move;
		}
		else if (to == G8 && from == E8 && board.CanCastle(BLACK, BLACK_KINGSIDE))
		{
			new_move.type = CASTLE_KINGSIDE;
            new_move.from = from;
            new_move.to = to;
			new_move.piece = BLACK_KING;
			return new_move;
		}
		else if (to == C8 && from == E8 && board.CanCastle(BLACK, BLACK_QUEENSIDE))
		{
			new_move.type = CASTLE_QUEENSIDE;
            new_move.from = from;
            new_move.to = to;
			new_move.piece = BLACK_KING;
			return new_move;
		}
	}

    // Find out the move by matching the source and destination squares
    // to a generated move.
    std::vector<Move> legal_moves;

    if(side == WHITE)
        move_generation::LegalAll<WHITE>(board,&legal_moves);
    else
        move_generation::LegalAll<BLACK>(board,&legal_moves);

    // Check that the move is a legal move.
    for(const Move& m : legal_moves)
    {
        if(m.from == from && m.to == to)
        {
            new_move = m;
            break;
        }
    }

    // Promotion move.
    PieceType promotion = PIECE_TYPE_NONE;
	if (move_str.size() == 5)
	{
        switch(move_str[4])
        {
            case 'k': promotion = piece_type_from_piece(KNIGHTS,side); break;
            case 'b': promotion = piece_type_from_piece(BISHOPS,side); break;
            case 'r': promotion = piece_type_from_piece(ROOKS,side); break;
            case 'q': promotion = piece_type_from_piece(QUEENS,side); break;
            default: break;
        }
	}
    new_move.promotion = promotion;

    return new_move;
}

void print_move(const Move& m)
{
    std::cout<<"From:\t"<<algebraic_from_square(m.from)<<"\n"<<
               "To:\t"<<algebraic_from_square(m.to)<<"\n"<<
               "Piece:\t"<<get_piece_name(m.piece)<<"\n"<<
               "Type:\t"<<get_move_type(m.type)<<"\n"<<
               "Promotion:\t"<<get_piece_name(m.piece)<<"\n"<<
               "Captured piece:\t"<<get_piece_name(m.captured_type)<<"\n"<<std::endl;
}

std::string get_move_type(MoveType movetype)
{
    switch(movetype)
    {
        case NORMAL: return "normal"; 
        case DOUBLE_PAWN: return "double pawn";
        case CASTLE_KINGSIDE: return "castle kingside";
        case CASTLE_QUEENSIDE: return "castle queenside";
		case PROMOTION: return "promotion";
	}

    return "";
}

std::string move_to_algebraic(const Move &m)
{
	std::string output("");

	output+= algebraic_from_square(m.from) + "";
	output+= algebraic_from_square(m.to);

    if(m.promotion != PIECE_TYPE_NONE)
	    output+= piece_character(m.promotion);

	return output;
}

std::vector<std::string> split_string(const std::string & input_string)
{
	return std::vector<std::string>();
}
