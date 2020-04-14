#ifndef UTIL_H_
#define UTIL_H_

#include "types.h"
#include "move.h"

#include <string>
#include <vector>

#define OPPONENT(side) ((side+1)%2)

// Returns the PieceType enum associated with the given
// piece character representation.
// Returns PIECE_TYPE_NONE for invalid characters.
PieceType piece_type_from_character(char character);

// Get character representation of the piece.
// If the piece isn't a valid piece the character '.' 
// is returned.
char piece_character(PieceType piece);

// Returns true if the given character 
// can be interpreted as a piece type character
bool is_valid_piece_character(char character);

// TODO switch the names of PieceType and Piece enums. 
bool piece_of_type(PieceType piece_type, Piece piece);

Side get_piece_color(PieceType piece);

inline Side get_opposing_side(Side side){return ((side == WHITE)?BLACK:WHITE);}

// Returns the algebraic notation string associated 
// with the given square enum.
// For example: A1 -> "A1"
// Returns an empty string for non-square values.
std::string algebraic_from_square(Square square);

// Opposite of above function.
Square square_from_algebraic(const std::string& square_str);


std::string get_piece_name(PieceType piece);

std::string castling_rights_to_string(u8 castling_rights);

// Returns true if the piece is one of
// Queen, Rook or Bishop
// and false otherwise
bool is_sliding_piece(PieceType piece);

std::string move_to_algebraic(const Move &m);

std::vector<std::string> split_string(const std::string& input_string, char delimiter);
#endif
