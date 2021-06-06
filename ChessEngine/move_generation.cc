#include "move_generation.h"
#include "bitboards.h"
#include "attacks.h"
#include "util.h"
#include "magic_bitboards.h"

#include <iostream>

using namespace magic_bitboards;

namespace move_generation
{

void Init()
{
    init_attacks();
	magic_bitboards::init();
}

void LegalAll(const Board& board, std::vector<Move>* move_list, Side side)
{
	// TODO optimize this better.
    std::vector<Move> pseudo_moves;

    PseudoLegalPawns(board,&pseudo_moves,side);
    PseudoLegalKnights(board,&pseudo_moves,side);
    PseudoLegalBishops(board,&pseudo_moves,side);
    PseudoLegalRooks(board,&pseudo_moves,side);
    PseudoLegalQueens(board,&pseudo_moves,side);
    PseudoLegalKings(board,&pseudo_moves,side);

    // Only add the moves that don't leave the king in check.
    Board test_board = board;
    for(Move m : pseudo_moves)
    {
        test_board.MakeMove(m);
        if(!test_board.InCheck((Side) OPPONENT(test_board.state_.side_to_move)))
            move_list->push_back(m);

        test_board.UndoMove();
    }
}

void PseudoLegalAll(const Board& board, std::vector<Move>* move_list, Side side)
{
    PseudoLegalPawns(board,move_list,side);
    PseudoLegalKnights(board,move_list,side);
    PseudoLegalBishops(board,move_list,side);
    PseudoLegalRooks(board,move_list,side);
    PseudoLegalQueens(board,move_list,side);
    PseudoLegalKings(board,move_list,side);
}

template<Side side>
void GeneratePawnMoves(const Board& board, std::vector<Move>* move_list)
{
    // TODO do as much as possible at compile time.
    /*
    constexpr Bitboard last_rank = (side == WHITE)?kBitboardRank8:kBitboardRank1;
    constexpr Bitboard second_rank = (side == WHITE)?kBitboardRank2:kBitboardRank7;
    constexpr Piece our_pawns = (side == WHITE)?WHITE_PAWNS:BLACK_PAWNS;
    constexpr Piece opponent_pawns = (side == WHITE)?BLACK_PAWNS:WHITE_PAWNS;

    Bitboard pawns = board.pieces_[our_pawns];

    while(pawns)
    {
        Square square = PopLSB(&pawns);
        Bitboard square_bb = BitboardFromSquare(square);

        if(square_bb & kLastRank) continue;

        Bitboard single_target = (side == WHITE) ? square_bb << 8 : square_bb >> 8;
        Bitboard double_target = (side == WHITE) ? square_bb << 16 : square_bb >> 16;

        Bitboard quiet_moves = (board.GetOccupied() & single_target) ^ single_target;
        
        Bitboard double_moves;
        if(side == WHITE)
        {
            double_moves = ((second_rank & square_bb) << 16) 
            & ((board.GetOccupied() & double_target) ^ (quiet_moves << 8));
        }
        else if(side == BLACK)
        {
            double_moves = ((second_rank & square_bb) >> 16) 
            & ((board.GetOccupied() & double_target) ^ (quiet_moves >> 8));
        }
    }
    */
}

void PseudoLegalPawns(const Board& board, std::vector<Move> *move_list, Side side)
{
    if(side == WHITE)
    {
        Bitboard pawns = board.pieces_[WHITE_PAWNS];
        while(pawns)
        {
            Square square = PopLSB(&pawns);
            Bitboard square_bb = bb_from_square(square);

            if(square_bb & kBitboardRank8) continue;

            Bitboard curr_target = square_bb<<8;
            Bitboard quiet_moves = ((board.GetOccupied()&curr_target) ^ (curr_target)); 
            curr_target = square_bb<<16;

			Bitboard double_moves = 0ULL;
			if (quiet_moves)
			{
				double_moves = ((kBitboardRank2&square_bb) << 16)
				&((board.GetOccupied()&curr_target) ^ (quiet_moves << 8));
			}

            curr_target = attacks[ATTACKS_WHITE_PAWN][square];
            Bitboard captures = (board.OccupiedBySide(BLACK)&curr_target); 

            Bitboard promotions = quiet_moves & kBitboardRank8;
            promotions |= captures & kBitboardRank8;

            // Add these moves only once.
            quiet_moves &= ~promotions;
            captures &= ~promotions;

            AddQuietMoves(square,quiet_moves,WHITE_PAWNS,move_list);
            AddDoublePawnMoves(square,double_moves, WHITE_PAWNS,move_list);
            AddCaptureMoves(square,captures,WHITE_PAWNS,move_list,board);
            AddPromotionMoves(square,promotions,WHITE_PAWNS,move_list,board);
        }
    }
    else
    {
        Bitboard pawns = board.pieces_[BLACK_PAWNS];
        while(pawns)
        {

            Square square = PopLSB(&pawns);
            Bitboard square_bb = bb_from_square(square);

            if(square_bb & kBitboardRank1) continue;

            Bitboard curr_target = square_bb>>8;
            Bitboard quiet_moves = ((board.GetOccupied()&curr_target) ^ (curr_target)); 
            
            curr_target = square_bb>>16;
			Bitboard double_moves = 0ULL;
			if (quiet_moves)
			{
				double_moves = ((kBitboardRank7&square_bb) >> 16)
					&((board.GetOccupied()&curr_target) ^ (quiet_moves >> 8));
			}

            curr_target = attacks[ATTACKS_BLACK_PAWN][square];
            Bitboard captures = (board.OccupiedBySide(WHITE)&curr_target); 

            Bitboard promotions = quiet_moves & kBitboardRank1;
            promotions |= captures & kBitboardRank1;

            quiet_moves &= ~promotions;
            captures &= ~promotions;

            AddQuietMoves(square,quiet_moves,BLACK_PAWNS,move_list);
            AddDoublePawnMoves(square,double_moves, BLACK_PAWNS,move_list);
            AddCaptureMoves(square,captures,BLACK_PAWNS,move_list,board);
            AddPromotionMoves(square,promotions,BLACK_PAWNS,move_list,board);
        }
    }
}

void PseudoLegalKnights(const Board& board, std::vector<Move>* move_list, Side side)
{
    if(side == WHITE)
    {
        Bitboard knights = board.pieces_[WHITE_KNIGHTS];
        while(knights)
        {
            Square square = PopLSB(&knights);
            Bitboard square_bb = bb_from_square(square);

            Bitboard targets = attacks[ATTACKS_KNIGHT][square];

            Bitboard quiet_moves = ((board.GetOccupied()&targets) ^ (targets)); 
            Bitboard captures = (board.OccupiedBySide(BLACK)&targets);

            AddQuietMoves(square,quiet_moves,WHITE_KNIGHTS,move_list);
            AddCaptureMoves(square,captures,WHITE_KNIGHTS,move_list,board);
        }
    }
    else
    {
        Bitboard knights = board.pieces_[BLACK_KNIGHTS];
        while(knights)
        {
            Square square = PopLSB(&knights);
            Bitboard square_bb = bb_from_square(square);

            Bitboard targets = attacks[ATTACKS_KNIGHT][square];
            Bitboard quiet_moves = ((board.GetOccupied()&targets) ^ (targets)); 
            Bitboard captures = (board.OccupiedBySide(WHITE)&targets);
            
            AddQuietMoves(square,quiet_moves,BLACK_KNIGHTS,move_list);
            AddCaptureMoves(square,captures,BLACK_KNIGHTS,move_list,board);
        }
    }
}

void PseudoLegalBishops(const Board& board, std::vector<Move>* move_list, Side side)
{
    if(side == WHITE)
    {
        Bitboard bishops = board.pieces_[WHITE_BISHOPS];
        while(bishops)
        {
            Square square = PopLSB(&bishops);
            Bitboard square_bb = bb_from_square(square);

            Bitboard targets = bishop_moves(board.GetOccupied(),square);

            targets &= ~board.OccupiedBySide(WHITE);

            Bitboard captures = (board.OccupiedBySide(BLACK)&targets);
            Bitboard quiet_moves = targets & ~captures; 

            AddQuietMoves(square,quiet_moves,WHITE_BISHOPS,move_list);
            AddCaptureMoves(square,captures,WHITE_BISHOPS,move_list,board);
        }
    }
    else
    {
        Bitboard bishops = board.pieces_[BLACK_BISHOPS];
        while(bishops)
        {
            Square square = PopLSB(&bishops);
            Bitboard square_bb = bb_from_square(square);

            Bitboard targets = bishop_moves(board.GetOccupied(),square);

            targets &= ~board.OccupiedBySide(BLACK);

            Bitboard captures = (board.OccupiedBySide(WHITE)&targets);
            Bitboard quiet_moves = targets & ~captures; 

            AddQuietMoves(square,quiet_moves,BLACK_BISHOPS,move_list);
            AddCaptureMoves(square,captures,BLACK_BISHOPS,move_list,board);
        }
    }
}
void PseudoLegalRooks(const Board& board, std::vector<Move>* move_list, Side side)
{
    if(side == WHITE)
    {
        Bitboard rooks = board.pieces_[WHITE_ROOKS];
        while(rooks)
        {
            Square square = PopLSB(&rooks);
            Bitboard square_bb = bb_from_square(square);

            Bitboard targets = rook_moves(board.GetOccupied(),square);
            targets &= ~board.OccupiedBySide(WHITE);

            Bitboard captures = (board.OccupiedBySide(BLACK)&targets);
            Bitboard quiet_moves = targets & ~captures; 

            AddQuietMoves(square,quiet_moves,WHITE_ROOKS,move_list);
            AddCaptureMoves(square,captures,WHITE_ROOKS,move_list,board);
        }
    }
    else
    {
        Bitboard rooks = board.pieces_[BLACK_ROOKS];
        while(rooks)
        {
            Square square = PopLSB(&rooks);
            Bitboard square_bb = bb_from_square(square);

            Bitboard targets = rook_moves(board.GetOccupied(),square);
            targets &= ~board.OccupiedBySide(BLACK);

            Bitboard captures = (board.OccupiedBySide(WHITE)&targets);
            Bitboard quiet_moves = targets & ~captures; 

            AddQuietMoves(square,quiet_moves,BLACK_ROOKS,move_list);
            AddCaptureMoves(square,captures,BLACK_ROOKS,move_list,board);
        }
    }
}
void PseudoLegalQueens(const Board& board, std::vector<Move>* move_list, Side side)
{
    if(side == WHITE)
    {
        Bitboard queens = board.pieces_[WHITE_QUEEN];
        while(queens)
        {
            Square square = PopLSB(&queens);
            Bitboard square_bb = bb_from_square(square);

            Bitboard targets = rook_moves(board.GetOccupied(),square)|bishop_moves(board.GetOccupied(),square);
            targets &= ~board.OccupiedBySide(WHITE);

            Bitboard captures = (board.OccupiedBySide(BLACK)&targets);
            Bitboard quiet_moves = targets & ~captures; 

            AddQuietMoves(square,quiet_moves,WHITE_QUEEN,move_list);
            AddCaptureMoves(square,captures,WHITE_QUEEN,move_list,board);
        }
    }
    else
    {
        Bitboard queens = board.pieces_[BLACK_QUEEN];
        while(queens)
        {
            Square square = PopLSB(&queens);
            Bitboard square_bb = bb_from_square(square);

            Bitboard targets = rook_moves(board.GetOccupied(),square)|bishop_moves(board.GetOccupied(),square);
            targets &= ~board.OccupiedBySide(BLACK);

            Bitboard captures = (board.OccupiedBySide(WHITE)&targets);
            Bitboard quiet_moves = targets & ~captures; 

            AddQuietMoves(square,quiet_moves,BLACK_QUEEN,move_list);
            AddCaptureMoves(square,captures,BLACK_QUEEN,move_list,board);
        }
    }
}

void PseudoLegalKings(const Board& board, std::vector<Move>* move_list, Side side)
{
    if(side == WHITE)
    {
        Bitboard king = board.pieces_[WHITE_KING];
        while(king)
        {
            Square square = PopLSB(&king);
            Bitboard square_bb = bb_from_square(square);

            Bitboard targets = attacks[ATTACKS_KING][square];
            targets &= ~board.OccupiedBySide(WHITE);

            Bitboard captures = (board.OccupiedBySide(BLACK)&targets);
            Bitboard quiet_moves = targets & ~captures; 

            AddQuietMoves(square,quiet_moves,WHITE_KING,move_list);
            AddCaptureMoves(square,captures,WHITE_KING,move_list,board);
        }

        if(board.CanCastle(side,WHITE_KINGSIDE))
        {
            AddCastlingMoves(WHITE_KINGSIDE,move_list);
        }
        if(board.CanCastle(side,WHITE_QUEENSIDE))
        {
            AddCastlingMoves(WHITE_QUEENSIDE,move_list);
        }
    }
    else
    {
        Bitboard king = board.pieces_[BLACK_KING];
        while(king)
        {
            Square square = PopLSB(&king);
            Bitboard square_bb = bb_from_square(square);

            Bitboard targets = attacks[ATTACKS_KING][square];
            targets &= ~board.OccupiedBySide(BLACK);

            Bitboard captures = (board.OccupiedBySide(WHITE)&targets);
            Bitboard quiet_moves = targets & ~captures; 

            AddQuietMoves(square,quiet_moves,BLACK_KING,move_list);
            AddCaptureMoves(square,captures,BLACK_KING,move_list,board);
        }

        if(board.CanCastle(side,BLACK_KINGSIDE))
        {
            AddCastlingMoves(BLACK_KINGSIDE,move_list);
        }
        if(board.CanCastle(side,BLACK_QUEENSIDE))
        {
            AddCastlingMoves(BLACK_QUEENSIDE,move_list);
        }
    }
}

void AddQuietMoves(Square from, Bitboard destinations, PieceType piece, std::vector<Move>* move_list)
{
    while(destinations)
    {
        Square to = PopLSB(&destinations);
        move_list->push_back({from,to,piece,NORMAL,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false});
    }
}

void AddDoublePawnMoves(Square from, Bitboard destinations, PieceType piece, std::vector<Move>* move_list)
{
    while(destinations)
    {
        Square to = PopLSB(&destinations);
        move_list->push_back({from,to,piece,DOUBLE_PAWN,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false});
    }
}

void AddCaptureMoves(Square from, Bitboard destinations, PieceType piece, std::vector<Move>* move_list, const Board& board)
{
    while(destinations)
    {
        Square to = PopLSB(&destinations);
        PieceType captured = board.GetPieceOnSquare(to); 

        move_list->push_back({from,to,piece,NORMAL,PIECE_TYPE_NONE,captured,true});
    }
}

void AddPromotionMoves(Square from, Bitboard destinations, PieceType piece, std::vector<Move>* move_list, const Board& board)
{
    while(destinations)
    {
        Square to = PopLSB(&destinations);

        // A promotion move can also be a capture move.
        PieceType captured_piece = board.GetPieceOnSquare(to);  

        int curr_promotion = WHITE_KNIGHTS;
        int end_promotion = WHITE_QUEEN;

        if(board.state_.side_to_move == BLACK)
        {
            curr_promotion = BLACK_KNIGHTS;
            end_promotion = BLACK_QUEEN;
        }

        for(;curr_promotion <= end_promotion; ++curr_promotion)
            move_list->push_back({from,to,piece,PROMOTION,(PieceType)curr_promotion,captured_piece,(captured_piece != PIECE_TYPE_NONE)});
    }
}

void AddCastlingMoves(Castling type, std::vector<Move>* move_list)
{
    switch(type)
    {
        case WHITE_KINGSIDE:
            move_list->push_back({F1,H1,WHITE_KING,CASTLE_KINGSIDE,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false});
            break;
        case WHITE_QUEENSIDE:
            move_list->push_back({D1,A1,WHITE_KING,CASTLE_QUEENSIDE,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false});
            break;
        case BLACK_KINGSIDE:
            move_list->push_back({F8,H8,BLACK_KING,CASTLE_KINGSIDE,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false});
            break;
        case BLACK_QUEENSIDE:
            move_list->push_back({D8,A8,BLACK_KING,CASTLE_QUEENSIDE,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false});
            break;
    }
}

};
