#include "util.h"
#include "bitboards.h"

#include <string>
#include <cassert>
#include <iostream>
#include <algorithm>

PieceType piece_type_from_character(char character)
{
    switch(character)
    {
        case 'P': return WHITE_PAWNS;
        case 'N': return WHITE_KNIGHTS;
        case 'B': return WHITE_BISHOPS;
        case 'R': return WHITE_ROOKS;
        case 'Q': return WHITE_QUEEN;
        case 'K': return WHITE_KING;
        case 'p': return BLACK_PAWNS;
        case 'n': return BLACK_KNIGHTS;
        case 'b': return BLACK_BISHOPS;
        case 'r': return BLACK_ROOKS;
        case 'q': return BLACK_QUEEN;
        case 'k': return BLACK_KING;
    }
    return PIECE_TYPE_NONE;
}

PieceType piece_type_from_string(const std::string& piece)
{
    if(piece == "white pawns") return WHITE_PAWNS;
    else if(piece == "white knights") return WHITE_KNIGHTS;
    else if(piece == "white bishops") return WHITE_BISHOPS;
    else if(piece == "white rooks") return WHITE_ROOKS;
    else if(piece == "white queen") return WHITE_QUEEN;
    else if(piece == "white king") return WHITE_KING;
    else if(piece == "black pawns") return BLACK_PAWNS;
    else if(piece == "black knights") return BLACK_KNIGHTS;
    else if(piece == "black bishops") return BLACK_BISHOPS;
    else if(piece == "black rooks") return BLACK_ROOKS;
    else if(piece == "black queen") return BLACK_QUEEN;
    else if(piece == "black king") return BLACK_KING;

    return PIECE_TYPE_NONE;
}

bool is_valid_piece_character(char character)
{
    for(int i=0;i<12;++i)
    {
        if("PNBRQKpnbrqk"[i] == character)
            return true;
    }
    return false;
}

char piece_character(PieceType piece)
{
	if (piece >= NUM_PIECE_TYPES) return '.';

	return "PNBRQKpnbrqk"[piece];
}

Side get_piece_color(PieceType piece)
{
    if(piece>=WHITE_PAWNS && piece<=WHITE_KING)
        return WHITE;
    else if(piece>=BLACK_PAWNS && piece<=BLACK_KING)
        return BLACK;
    else
        return SIDE_NONE;
}

std::string algebraic_from_square(Square square)
{
    if(square < 0 || square >= NUM_SQUARES)
        return "";

    File file = GetSquareFile(square);
    Rank rank = GetSquareRank(square);

    assert(file >= FILE_A && file <= NUM_FILES);
    assert(rank >= RANK_1 && rank <= NUM_RANKS);

    static const char *files = "ABCDEFGH";
    static const char *ranks = "12345678";
    return std::string(std::string("") + files[(int)file] + ranks[(int)rank]);
}

std::string get_piece_name(PieceType piece)
{
    if(piece == PIECE_TYPE_NONE) return "none";

    std::string color = (piece >= WHITE_PAWNS && piece <= WHITE_KING)?"White ":"Black ";
    std::string piece_type_name("");
    switch(piece%NUM_PIECES)
    {
        case 0: piece_type_name+="Pawn"; break;
        case 1: piece_type_name+="Knight"; break;
        case 2: piece_type_name+="Bishop"; break;
        case 3: piece_type_name+="Rook"; break;
        case 4: piece_type_name+="Queen"; break;
        case 5: piece_type_name+="King"; break;
    }

    return color + piece_type_name;
}

std::string castling_rights_to_string(u8 castling_rights)
{
    std::string str("....");
    if(castling_rights & BLACK_QUEENSIDE) str[0] = 'q';
    if(castling_rights & BLACK_KINGSIDE) str[1] = 'k';
    if(castling_rights & WHITE_QUEENSIDE) str[2] = 'Q';
    if(castling_rights & WHITE_KINGSIDE) str[3] = 'K';

    return str;
}

bool is_sliding_piece(PieceType piece)
{
    switch(piece)
    {
        case WHITE_BISHOPS:
        case BLACK_BISHOPS:
        case WHITE_ROOKS:
        case BLACK_ROOKS:
        case WHITE_QUEEN:
        case BLACK_QUEEN:
            return true;
            break;
        default:
            return false;
            break;
    }
}

Square square_from_algebraic(const std::string& square_str)
{
    Bitboard rank_bb = kBitboardRank1, 
             file_bb = kBitboardFileA;    

    if(square_str.size() != 2) 
        return SQUARE_NONE;

    for(int i=0;i<NUM_FILES;++i, file_bb>>=1)
    {
        if(square_str[0] == "abcdefgh"[i]) break;
    }

    for(int i=0;i<NUM_RANKS;++i, rank_bb<<=8)
    {
        if(square_str[1] == "12345678"[i]) break;
    }

    return square_from_bitboard(rank_bb & file_bb);
}

std::vector<std::string> split_string(const std::string& input_string, char delimiter)
{
	std::vector<std::string> split_string;

	std::string curr_substring(""); 
	for (char c : input_string)
	{
		if (c == delimiter)
		{
			split_string.push_back(curr_substring);
			curr_substring.clear();
			continue;
		}

		curr_substring += c;
	}

	return split_string;
}

std::string trim_string(std::string *s)
{
    s->erase(s->begin(), std::find_if(s->begin(), s->end(), [](unsigned char ch){
        return !std::isspace(ch);             
    }));

    s->erase(std::find_if(s->rbegin(), s->rend(), [](unsigned char ch){
        return !std::isspace(ch);             
    }).base(), s->end());

    return *s;
}

bool piece_of_type(PieceType piece_type, Piece piece)
{
	switch (piece_type)
	{
		case WHITE_PAWNS:
		case BLACK_PAWNS: return (piece == PAWNS);

		case WHITE_KNIGHTS:
		case BLACK_KNIGHTS: return (piece == KNIGHTS);

		case WHITE_BISHOPS:
		case BLACK_BISHOPS: return (piece == BISHOPS);

		case WHITE_ROOKS:
		case BLACK_ROOKS: return (piece == ROOKS);

		case WHITE_QUEEN:
		case BLACK_QUEEN: return (piece == QUEENS);

		case WHITE_KING:
		case BLACK_KING: return (piece == KINGS);

		default:
			return false;
	}
}

PieceType piece_type_from_piece(Piece piece, Side side)
{
    switch(piece)
    {
        case PAWNS:         return (side==WHITE)?WHITE_PAWNS:BLACK_PAWNS;
        case KNIGHTS:       return (side==WHITE)?WHITE_KNIGHTS:BLACK_KNIGHTS;
        case BISHOPS:       return (side==WHITE)?WHITE_BISHOPS:BLACK_BISHOPS;
        case ROOKS:         return (side==WHITE)?WHITE_ROOKS:BLACK_ROOKS;
        case QUEENS:        return (side==WHITE)?WHITE_QUEEN:BLACK_QUEEN;
        case KINGS:         return (side==WHITE)?WHITE_KING:BLACK_KING;
        default:            return PIECE_TYPE_NONE;
    }
}
