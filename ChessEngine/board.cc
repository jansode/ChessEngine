#include "board.h"
#include "bitboards.h"
#include "util.h"
#include "attacks.h"
#include "evaluate.h"
#include "magic_bitboards.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>

u64 Board::zobrist_table[NUM_PIECE_TYPES][NUM_SQUARES];

//TODO fix this rng stuff
std::random_device rd2; 
std::mt19937 gen(rd2());

using namespace magic_bitboards;

Board::Board()
{
    Reset();
}

Board::Board(const Board& board)
{
    *this = board;
}

Board& Board::operator=(const Board& board)
{
    for(int i=0; i<NUM_PIECE_TYPES; ++i)
        this->pieces_[i] = board.pieces_[i];

    this->state_ = board.state_;
    this->history_ = board.history_;

    return *this;
}

void Board::Reset()
{
    state_.hash = 0;
    state_.side_to_move = WHITE;
    state_.castling_rights = 0xF;
    state_.half_moves = 0;
    state_.full_moves = 1;
    state_.en_passant_square = SQUARE_NONE;
    state_.king_has_moved[0] = false;
    state_.king_has_moved[1] = false;

    std::fill(pieces_,pieces_+NUM_PIECE_TYPES,0);
}

void Board::Reset(Bitboard *pieces, const State& state)
{ 
    state_ = state;

    for(int i = WHITE_PAWNS; i < NUM_PIECE_TYPES; ++i)
        pieces_[i] = *(pieces+i);
}

bool Board::SetPositionFromFEN(const std::string& fen_string)
{
    // Take copies of board state so that the board
    // can be reset if an error occurs.
    /*
    Bitboard pieces_copy[NUM_PIECE_TYPES];
    State state_copy = state_;

    for(int piece = WHITE_PAWNS; piece < NUM_PIECE_TYPES; ++piece)
        pieces_copy[piece] = pieces_[piece];
        */

    Reset();
    state_.castling_rights = 0;

    Bitboard curr_square = kBitboardRank8 & kBitboardFileA;

    enum State{POSITION,SIDE,CASTLING,EN_PASSANT,HALF_MOVES,FULL_MOVES};
    int curr_state = POSITION;

    for(int i=0;i<fen_string.size();++i)
    {
        switch(curr_state)
        {
            case POSITION:
            {
                if(fen_string[i] == ' ')
                {
                    curr_state = SIDE;
                    continue;
                }

                if(is_valid_piece_character(fen_string[i]))
                {
                    pieces_[(int)piece_type_from_character(fen_string[i])] |= curr_square;
                    curr_square >>= 1;
                    continue;
                }

                int number = fen_string[i] - '0';
                if(number >= 0 && number <= NUM_FILES)
                    curr_square >>= number;
                break;
            }
            case SIDE: 
            {
                if(fen_string[i] == ' ')
                {
                    curr_state = CASTLING;
                    continue;
                }

                state_.side_to_move = (fen_string[i] == 'w')?WHITE:BLACK;
                break;
            }
            case CASTLING:
            {
                if(fen_string[i] == ' ')
                {
                    curr_state = EN_PASSANT;
                    continue;
                }

                if(fen_string[i] == '-')
                    continue;
                else
                {
                    switch(fen_string[i]) 
                    {
                        case 'K': state_.castling_rights |= WHITE_KINGSIDE; break;
                        case 'Q': state_.castling_rights |= WHITE_QUEENSIDE; break;
                        case 'k': state_.castling_rights |= BLACK_KINGSIDE; break;
                        case 'q': state_.castling_rights |= BLACK_QUEENSIDE; break;
                    }
                }
                break;
            }
            case EN_PASSANT:
            {
                if(fen_string[i] == ' ')
                {
                    curr_state = HALF_MOVES;
                    continue;
                }

                if(fen_string[i] == '-')
                {
                    state_.en_passant_square = SQUARE_NONE;
                }
                else
                {
                    state_.en_passant_square = square_from_bitboard(BitboardFromAlgebraic(std::string() + fen_string[i] + fen_string[++i])); 
                }
                break;
            }
            case HALF_MOVES:
            {
                std::string half_moves(""); 
                for(int j=0;j<3;++j)
                {
					int index = i + j;
					if (index == fen_string.size() || fen_string[index] == ' ')
                    {
                        i+=j;
                        break;
                    }
                    half_moves+=fen_string[i+j];
                }

                state_.half_moves = std::stoi(half_moves,nullptr);
                curr_state = FULL_MOVES;
                break;
            }
            case FULL_MOVES:
            {
                std::string full_moves(""); 
                for(int j=0;j<3;++j)
                {
					int index = i + j;
                    if(index == fen_string.size() || fen_string[index] == ' ')
                    {
                        i+=j;
                        break;
                    }
                    full_moves+=fen_string[i+j];
                }

                state_.full_moves= std::stoi(full_moves,nullptr);
                break;
            }
        }
    }

	if (curr_state != FULL_MOVES) 
    {
        //Reset(pieces_copy,state_copy);
        return false;
    }

    UpdateZobristHash(); 
	return true;
}

std::string Board::GenerateFenString()
{
	std::stringstream fen;

	int empty_squares = 0;
	for (int curr_square = A8; curr_square < SQUARE_NONE; ++curr_square)
	{
		if ((curr_square != 0) && ((curr_square % NUM_FILES) == 0))
		{
			if (empty_squares)
			{
				fen << empty_squares;
				empty_squares = 0;
			}
            
            if(curr_square != NUM_SQUARES)
                fen << '/';
		}

		PieceType curr_piece = PIECE_TYPE_NONE;
		if ((curr_piece = this->GetPieceOnSquare((Square)curr_square)) != PIECE_TYPE_NONE)
		{
			if (empty_squares)
			{
				fen << empty_squares;
				empty_squares = 0;
			}

			char character = piece_character(curr_piece);
			fen << character;
		}
		else 
			++empty_squares;
	}

	fen << ' ' << ((state_.side_to_move == WHITE) ? "w " : "b ");

	if (state_.castling_rights == 0)
		fen << "-";
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			if (state_.castling_rights & (0x1 << i))
				fen << "qkQK"[i];
		}
	}

	if (state_.en_passant_square != SQUARE_NONE)
		fen << ' ' << algebraic_from_square((Square)state_.en_passant_square);
	else
		fen << " -";

	fen << " " << state_.half_moves;
	fen << " " << state_.full_moves;

	return fen.str();
}

void Board::MakeMove(Move move)
{
	if (piece_of_type(move.piece, KINGS) && !state_.king_has_moved[state_.side_to_move])
	{
		state_.king_has_moved[state_.side_to_move] = true;
	}

    history_.push_back({state_,move});

    Side side = state_.side_to_move;
    state_.side_to_move = ((state_.side_to_move == WHITE)?BLACK:WHITE);

    state_.en_passant_square = SQUARE_NONE;
    state_.half_moves++;
    if(state_.full_moves != 1 && side == WHITE) state_.full_moves++;

    switch(move.type)
    {
        case DOUBLE_PAWN:
        {
            // Set en passant square
            // if the move is a double
            // pawn move. The en passant
            // square is the square behind 
            // the destination square.
            state_.en_passant_square = (Square)((side == WHITE)?move.to-8:move.to+8);
            break;
        }
        // The rook moves for 
        // castling is handled 
        // like a normal move.
        // These switch cases move 
        // the king.
        case CASTLE_KINGSIDE:
        {
            if(side == WHITE) 
                state_.castling_rights &= ~WHITE_KINGSIDE;
            else 
                state_.castling_rights &= ~BLACK_KINGSIDE;
            break;
        }
        case CASTLE_QUEENSIDE:
        {
            if(side == WHITE) 
                state_.castling_rights &= ~WHITE_QUEENSIDE;
            else 
                state_.castling_rights &= ~BLACK_QUEENSIDE;
            break;
        }
    }

    // Update pieces 
    Bitboard from = bb_from_square(move.from);
    Bitboard to = bb_from_square(move.to);
    Bitboard from_to_bb = from | to;
    pieces_[move.piece] ^= from_to_bb;

    if(move.capture) pieces_[move.captured_type] &= ~to;
    if(move.promotion != PIECE_TYPE_NONE) 
    {
        pieces_[move.piece] &= ~to;
        pieces_[move.promotion] |= to;
    }
}

void Board::UndoMove()
{
    // This function is basically 
    // the MakeMove function in 
    // reverse. 
   
    Undo undo_info = history_.back();
    history_.pop_back();

    state_ = undo_info.state;

    Move undo_move = undo_info.move; 

    switch(undo_move.type)
    {
        case CASTLE_KINGSIDE:
        {
            if(state_.side_to_move == WHITE) pieces_[WHITE_KING] <<= 2;
            else pieces_[BLACK_KING] <<= 2;
            break;
        }
        case CASTLE_QUEENSIDE:
        {
            if(state_.side_to_move == WHITE) pieces_[WHITE_KING] >>= 2;
            else pieces_[BLACK_KING] >>= 2;
            break;
        }
    }

    // Update pieces 
    Bitboard from = bb_from_square(undo_move.from);
    Bitboard to = bb_from_square(undo_move.to);
    Bitboard from_to_bb = from | to;
    pieces_[undo_move.piece] ^= from_to_bb;

    if(undo_move.capture) pieces_[undo_move.captured_type] |= to;
    if(undo_move.promotion != PIECE_TYPE_NONE) 
    {
        pieces_[undo_move.promotion] &= ~to;
    }
}

bool Board::SquareAttacked(Square square, Side side) const
{
    Bitboard square_bb = bb_from_square(square);

    Side opponent = get_opposing_side(side);
    Bitboard occupied_opponent = OccupiedBySide(opponent);
    Bitboard occupied = GetOccupied();

    // For each opponent piece, compare their attacks 
    // to the square. Make a copy of the occupy mask 
    // because we need the unmodified one inside the loop.
    Bitboard opponent_squares = occupied_opponent;
    while(opponent_squares)
    {
        Square piece_square = PopLSB(&opponent_squares);
        PieceType piece = GetPieceOnSquare(piece_square);

        Bitboard piece_square_bb = bb_from_square(piece_square);

        Bitboard attacked_squares = 0ULL;
        if(is_sliding_piece(piece))
        {
            if(piece == WHITE_BISHOPS || piece == BLACK_BISHOPS) 
                attacked_squares = bishop_moves(occupied,piece_square);
            else if(piece == WHITE_ROOKS || piece == BLACK_ROOKS)
                attacked_squares = rook_moves(occupied,piece_square);
            else if(piece == WHITE_QUEEN || piece == BLACK_QUEEN)
                attacked_squares = queen_moves(occupied,piece_square);
        }
        else
        {
			int attacks_index = piece_type_to_attacks_index(piece);
            attacked_squares = attacks[attacks_index][piece_square];
        }

        attacked_squares &= ~occupied_opponent;
        if(attacked_squares & square_bb) return true;
    }
    return false;
}

bool Board::InCheck(Side side) const
{
    PieceType piece = (side == WHITE)?WHITE_KING:BLACK_KING;
    Square king_square = square_from_bitboard(pieces_[(int)piece]);

    return SquareAttacked(king_square,side);
}

Bitboard Board::OccupiedBySide(Side side) const
{
    Bitboard occupied = 0;

    int start_piece = (side == WHITE)?WHITE_PAWNS:BLACK_PAWNS;
    int end_piece = (side == WHITE)?WHITE_KING:BLACK_KING;
    for(int curr_piece = start_piece; curr_piece <= end_piece; ++curr_piece)
    {
       occupied |= pieces_[curr_piece]; 
    }

    return occupied; 
}

Bitboard Board::GetOccupied() const
{
    Bitboard occupied = 0; 
    for(int curr_piece = WHITE_PAWNS; curr_piece < NUM_PIECE_TYPES; ++curr_piece)
    {
        occupied |= pieces_[curr_piece];
    }
    return occupied;
}

PieceType Board::GetPieceOnSquare(Bitboard square) const
{
	for (int piece = WHITE_PAWNS; piece <= BLACK_KING; ++piece)
	{
		if (pieces_[piece] & square) 
			return (PieceType)piece;
	}

    return PIECE_TYPE_NONE;
}

PieceType Board::GetPieceOnSquare(Square square) const
{
    return GetPieceOnSquare(bb_from_square(square));
}

bool Board::CanCastle(Side side, Castling type) const
{
	if (state_.king_has_moved[side]) return false;

    if(InCheck(side)) 
        return false;

	if(!(state_.castling_rights & (int)type)) return false;

    Bitboard occupied = GetOccupied();

    bool rook_exists = true; 
    switch(type)
    {
        case BLACK_QUEENSIDE:
			rook_exists = pieces_[BLACK_ROOKS] & bb_from_square(A8);
            return !(kCastlingSquaresBQ & occupied) && rook_exists;
            break;
        case BLACK_KINGSIDE:
            rook_exists = pieces_[BLACK_ROOKS] & bb_from_square(H8);
            return !(kCastlingSquaresBK & occupied) && rook_exists;
            break;
        case WHITE_QUEENSIDE:
            rook_exists = pieces_[WHITE_ROOKS] & bb_from_square(A1);
            return !(kCastlingSquaresWQ & occupied) && rook_exists;
            break;
        case WHITE_KINGSIDE:
            rook_exists = pieces_[WHITE_ROOKS] & bb_from_square(H1);
            return !(kCastlingSquaresWK & occupied) && rook_exists;
            break;

    }

    return true;
}

void Board::PrintPosition() const
{
	const char *files = "ABCDEFGH";
	const char *ranks = "87654321";
	int curr_rank = RANK_1;

    const char *letters = "PNBRQKpnbrqk..";
	std::cout <<ranks[curr_rank++] << " ";
    for(Bitboard square_bb=kBitboardMSB;square_bb>0;square_bb>>=1)     
    {
        std::cout<<letters[GetPieceOnSquare(square_bb)];
        if(square_bb & kBitboardFileH) std::cout<<"\n"  << ranks[curr_rank++] << " ";
    }

    std::cout<<" ";

	for (int file = FILE_A; file < NUM_FILES; ++file)
	{
		std::cout << files[file];
	}
	std::cout << "\n";
}

void Board::PrintState() const
{
    std::cout<<"Side to move: "<<((state_.side_to_move == WHITE)?"white\n":"black\n");
    std::cout<<"Castling rights: "<<castling_rights_to_string(state_.castling_rights)<<'\n';
    std::cout<<"Half moves: "<<state_.half_moves<<'\n';
    std::cout<<"Full moves: "<<state_.full_moves<<'\n';
    std::cout<<"En passant square: "<<algebraic_from_square(state_.en_passant_square)<<'\n';
}

void Board::InitZobristHashing()
{
    for(int piece_type = 0; piece_type < NUM_PIECE_TYPES; ++piece_type)  
    {
        for(int square = 0; square < NUM_SQUARES; ++square)
        {
            std::uniform_int_distribution<u64> dist(0,UINT64_MAX);
            zobrist_table[piece_type][square] = dist(gen);
        }
    }
}

void Board::UpdateZobristHash()
{
    state_.hash = 0ULL;
    for(int square=0; square<NUM_SQUARES; ++square)
    {
        int occupying_piece = GetPieceOnSquare(square);
        if(occupying_piece != PIECE_TYPE_NONE)
            state_.hash ^= zobrist_table[occupying_piece][square];
    }
}

int Board::Evaluate()
{
	return evaluation::evaluate(this);
}

