#ifndef MOVE_GENERATION_H_
#define MOVE_GENERATION_H_ 

#include <vector>

#include "move.h"
#include "board.h"

namespace move_generation 
{
    void Init();

    //template <PieceType piece> 

    //void Generate(const Board& board, std::vector<Move>* move_list);

    template<Side side>
    void LegalAll(const Board& board, std::vector<Move>* move_list);

    template<Side side>
    void PseudoLegalAll(const Board& board, std::vector<Move>* move_list);

    template<Side side>
    void PseudoLegalPawns(const Board& board, std::vector<Move>* move_list);

    //void GeneratePseudoLegal(const Board& board, std::vector<Move>* move_list);

    //void PseudoLegalPawns(const Board& board, std::vector<Move>* move_list, Side side);
    void PseudoLegalKnights(const Board& board, std::vector<Move>* move_list, Side side);

    void PseudoLegalBishops(const Board& board, std::vector<Move>* move_list, Side side);

    void PseudoLegalRooks(const Board& board, std::vector<Move>* move_list, Side side);

    void PseudoLegalQueens(const Board& board, std::vector<Move>* move_list, Side side);

    void PseudoLegalKings(const Board& board, std::vector<Move>* move_list, Side side);

    void AddQuietMoves(Square from, Bitboard destinations, PieceType piece, std::vector<Move>* move_list);

    void AddDoublePawnMoves(Square from, Bitboard destinations, PieceType piece, std::vector<Move>* move_list);

    void AddCaptureMoves(Square from, Bitboard destinations, PieceType piece, std::vector<Move>* move_list, const Board& board);

    void AddPromotionMoves(Square from, Bitboard destinations, PieceType piece, std::vector<Move> *move_list, const Board& board);

    void AddCastlingMoves(Castling type, std::vector<Move>* move_list);

};

#endif // MOVE_GENERATION_H_

