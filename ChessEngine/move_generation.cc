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

template <Side side>
void LegalAll(const Board& board, std::vector<Move>* move_list)
{
	// TODO optimize this better.
    std::vector<Move> pseudo_moves;
    PseudoLegalAll<side>(board,&pseudo_moves);

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

template void LegalAll<WHITE>(const Board& board, std::vector<Move>* move_list);
template void LegalAll<BLACK>(const Board& board, std::vector<Move>* move_list);

template<Side side>
void PseudoLegalPawns(const Board& board, std::vector<Move>* move_list)
{
    constexpr Side opponent = (side == WHITE)?BLACK:WHITE;
    constexpr Bitboard last_rank = (side == WHITE)?kBitboardRank8:kBitboardRank1;
    constexpr Bitboard second_rank = (side == WHITE)?kBitboardRank2:kBitboardRank7;
    constexpr PieceAttacks attacks_piece = (side == WHITE)?ATTACKS_WHITE_PAWN:ATTACKS_BLACK_PAWN;
    constexpr PieceType our_pawns = (side == WHITE)?WHITE_PAWNS:BLACK_PAWNS;
    constexpr PieceType opponent_pawns = (side == WHITE)?BLACK_PAWNS:WHITE_PAWNS;

    Bitboard pawns = board.pieces_[our_pawns];

    while(pawns)
    {
        Square square = PopLSB(&pawns);
        Bitboard square_bb = bb_from_square(square);

        // Impossible situation. The pawn has to promote on the last rank.
        if(square_bb & last_rank) continue;

        Bitboard single_target = (side == WHITE) ? square_bb << 8 : square_bb >> 8;
        Bitboard double_target = (side == WHITE) ? square_bb << 16 : square_bb >> 16;

        Bitboard quiet_moves = (board.GetOccupied() & single_target) ^ single_target;

        Bitboard double_moves = 0ULL;
        Bitboard on_second_rank = second_rank & square_bb;
        Bitboard target_empty = ~(board.GetOccupied() & double_target);
        if constexpr (side == WHITE)
        {
            double_moves = (on_second_rank << 16) 
            & (target_empty & (quiet_moves << 8));
        }
        else if(side == BLACK)
        {
            double_moves = (on_second_rank >> 16) 
            & (target_empty & (quiet_moves >> 8));
        }

        Bitboard attacks_target = attacks[attacks_piece][square];

        Bitboard captures = (board.OccupiedBySide(opponent)&attacks_target);

        Bitboard promotions = quiet_moves & last_rank;
        promotions |= captures & last_rank;

        quiet_moves &= ~promotions;
        captures &= ~promotions;

        AddQuietMoves(square,quiet_moves,our_pawns,move_list);
        AddDoublePawnMoves(square,double_moves, our_pawns,move_list);
        AddCaptureMoves(square,captures,our_pawns,move_list,board);
        AddPromotionMoves(square,promotions,our_pawns,move_list,board);
    }
}

template void PseudoLegalPawns<WHITE>(const Board &board, std::vector<Move>* move_list);
template void PseudoLegalPawns<BLACK>(const Board &board, std::vector<Move>* move_list);

template <Side side>
void PseudoLegalAll(const Board& board, std::vector<Move>* move_list)
{
    PseudoLegalPawns<side>(board,move_list);
    PseudoLegalKnights<side>(board,move_list);
    PseudoLegalBishops<side>(board,move_list);
    PseudoLegalRooks<side>(board,move_list);
    PseudoLegalQueens<side>(board,move_list);
    PseudoLegalKings<side>(board,move_list);
}

template void PseudoLegalAll<WHITE>(const Board& board, std::vector<Move>* move_list);
template void PseudoLegalAll<BLACK>(const Board& board, std::vector<Move>* move_list);

template<Side side>
void PseudoLegalKnights(const Board& board, std::vector<Move>* move_list)
{
    constexpr PieceType pieces = (side == WHITE)?WHITE_KNIGHTS:BLACK_KNIGHTS;
    constexpr Side opponent = (side==WHITE)?BLACK:WHITE;

    Bitboard knights = board.pieces_[pieces];
    while(knights)
    {
        Square square = PopLSB(&knights);
        Bitboard square_bb = bb_from_square(square);
        
        Bitboard targets = attacks[ATTACKS_KNIGHT][square];
        Bitboard quiet_moves = ((board.GetOccupied()&targets) ^ (targets)); 
        Bitboard captures = (board.OccupiedBySide(opponent)&targets);

        AddQuietMoves(square,quiet_moves,pieces,move_list);
        AddCaptureMoves(square,captures,pieces,move_list,board);
    }
}

template void PseudoLegalKnights<WHITE>(const Board& board, std::vector<Move>* move_list);
template void PseudoLegalKnights<BLACK>(const Board& board, std::vector<Move>* move_list);

template<Side side>
void PseudoLegalBishops(const Board& board, std::vector<Move>* move_list)
{
    constexpr PieceType pieces = (side == WHITE)?WHITE_BISHOPS:BLACK_BISHOPS;
    constexpr Side opponent = (side == WHITE)?BLACK:WHITE;
    
    Bitboard bishops = board.pieces_[pieces];

    while(bishops)
    {
        Square square = PopLSB(&bishops);
        Bitboard square_bb = bb_from_square(square);

        Bitboard targets = bishop_moves(board.GetOccupied(),square);
        targets &= ~board.OccupiedBySide(side);

        Bitboard captures = (board.OccupiedBySide(opponent)&targets);
        Bitboard quiet_moves = targets & ~captures; 

        AddQuietMoves(square,quiet_moves,pieces,move_list);
        AddCaptureMoves(square,captures,pieces,move_list,board);
    }
}

template void PseudoLegalBishops<WHITE>(const Board& board, std::vector<Move>* move_list);
template void PseudoLegalBishops<BLACK>(const Board& board, std::vector<Move>* move_list);

template <Side side>
void PseudoLegalRooks(const Board& board, std::vector<Move>* move_list)
{
    constexpr PieceType pieces = (side == WHITE)?WHITE_ROOKS:BLACK_ROOKS;
    constexpr Side opponent = (side == WHITE)?BLACK:WHITE;

    Bitboard rooks = board.pieces_[pieces];
    while(rooks)
    {
        Square square = PopLSB(&rooks);
        Bitboard square_bb = bb_from_square(square);

        Bitboard targets = rook_moves(board.GetOccupied(),square);
        targets &= ~board.OccupiedBySide(side);

        Bitboard captures = (board.OccupiedBySide(opponent)&targets);
        Bitboard quiet_moves = targets & ~captures; 

        AddQuietMoves(square,quiet_moves,pieces,move_list);
        AddCaptureMoves(square,captures,pieces,move_list,board);
    }
}

template void PseudoLegalRooks<WHITE>(const Board& board, std::vector<Move>* move_list);
template void PseudoLegalRooks<BLACK>(const Board& board, std::vector<Move>* move_list);

template <Side side>
void PseudoLegalQueens(const Board& board, std::vector<Move>* move_list)
{
    constexpr PieceType pieces = (side == WHITE)?WHITE_QUEEN:BLACK_QUEEN;
    constexpr Side opponent = (side == WHITE)?BLACK:WHITE;

    Bitboard queens = board.pieces_[pieces];

    while(queens)
    {
        Square square = PopLSB(&queens);
        Bitboard square_bb = bb_from_square(square);

        Bitboard targets = rook_moves(board.GetOccupied(),square)|bishop_moves(board.GetOccupied(),square);
        targets &= ~board.OccupiedBySide(side);

        Bitboard captures = (board.OccupiedBySide(opponent)&targets);
        Bitboard quiet_moves = targets & ~captures; 

        AddQuietMoves(square,quiet_moves,pieces,move_list);
        AddCaptureMoves(square,captures,pieces,move_list,board);
    }
}

template void PseudoLegalQueens<WHITE>(const Board& board, std::vector<Move>* move_list);
template void PseudoLegalQueens<BLACK>(const Board& board, std::vector<Move>* move_list);

template <Side side>
void PseudoLegalKings(const Board& board, std::vector<Move>* move_list)
{
    constexpr PieceType pieces = (side == WHITE)?WHITE_KING:BLACK_KING;
    constexpr Castling castling_ks = (side == WHITE)?WHITE_KINGSIDE:BLACK_KINGSIDE;
    constexpr Castling castling_qs = (side == WHITE)?WHITE_QUEENSIDE:BLACK_QUEENSIDE;
    constexpr Side opponent = (side == WHITE)?BLACK:WHITE;

    Bitboard king = board.pieces_[pieces];

    while(king)
    {
        Square square = PopLSB(&king);
        Bitboard square_bb = bb_from_square(square);

        Bitboard targets = attacks[ATTACKS_KING][square];
        targets &= ~board.OccupiedBySide(side);

        Bitboard captures = (board.OccupiedBySide(opponent)&targets);
        Bitboard quiet_moves = targets & ~captures; 

        AddQuietMoves(square,quiet_moves,pieces,move_list);
        AddCaptureMoves(square,captures,pieces,move_list,board);
    }

    if(board.CanCastle(side,castling_ks))
    {
        AddCastlingMoves(castling_ks,move_list);
    }
    if(board.CanCastle(side,castling_qs))
    {
        AddCastlingMoves(castling_qs,move_list);
    }
}

template void PseudoLegalKings<WHITE>(const Board& board, std::vector<Move>* move_list);
template void PseudoLegalKings<BLACK>(const Board& board, std::vector<Move>* move_list);

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
            move_list->push_back({E1,G1,WHITE_KING,CASTLE_KINGSIDE,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false});
            break;
        case WHITE_QUEENSIDE:
            move_list->push_back({E1,C1,WHITE_KING,CASTLE_QUEENSIDE,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false});
            break;
        case BLACK_KINGSIDE:
            move_list->push_back({E8,G8,BLACK_KING,CASTLE_KINGSIDE,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false});
            break;
        case BLACK_QUEENSIDE:
            move_list->push_back({E8,C8,BLACK_KING,CASTLE_QUEENSIDE,PIECE_TYPE_NONE,PIECE_TYPE_NONE,false});
            break;
    }
}

};
