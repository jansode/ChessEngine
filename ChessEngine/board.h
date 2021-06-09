#ifndef BOARD_H_
#define BOARD_H_

#include "types.h"
#include "move.h"

#include <string>
#include <vector>

struct State
{
    u64         hash;
    Side        side_to_move;
    u8          castling_rights;
    unsigned    half_moves;
    unsigned    full_moves;
    Square      en_passant_square;

	// Keep track of whether the king has moved or not.
	// This is needed in the castling checks.
	bool king_has_moved[NUM_SIDES] = {false, false};
};

// An element on the history stack.
struct Undo
{
    State   state;
    Move    move;
};

class Board
{

public:
    Board();
    Board(const Board& board);
    Board& operator=(const Board& board);

    static void InitZobristHashing();

    // Updates the hash in state_
    // to reflect the current position.
    void UpdateZobristHash();

    // Resets the board to initial values.
    // The piece bitboards are reset to zero.
    void Reset();

    // Resets the board to the given state.
    // The pieces pointer should point to a 
    // similar array like the pieces_ member variable.
    void Reset(Bitboard *pieces, const State& state);

    // Prints the current position to stdout.
    void PrintPosition() const;

    // Prints the current board state to stdout.
    void PrintState() const;

    // Sets the position according to the given fen string.
	// Returns false if position could not be parsed.
    bool SetPositionFromFEN(const std::string& fen_string);

	// Generate FEN string from current position.
	std::string GenerateFenString();

    // Makes a move on the board.
    void MakeMove(Move move);

    // Undoes the last move that was made.
    void UndoMove();
    
    // Returns a heuristic value for the current position.
    int Evaluate();

    // Returns true if the square for the given side is being attacked
    // by the opponent.
    bool SquareAttacked(Square square, Side side) const;

    // Returns true if the given side is in check.
    bool InCheck(Side side) const;

    // Returns true if castling is allowed for the 
    // given side and castling type.
    bool CanCastle(Side side, Castling type) const;

    // Returns a bitboard of the squares occupied 
    // by the given side.
    Bitboard OccupiedBySide(Side side) const; 

    // Returns a bitboard of all the occupied squares.
    Bitboard GetOccupied() const;

	Side SideToMove() const {return state_.side_to_move;} 

    // Returns the enumeration of the piece that occupies the given square.
    PieceType GetPieceOnSquare(Bitboard square) const;
    PieceType GetPieceOnSquare(Square square) const;

    Bitboard pieces_[NUM_PIECE_TYPES];
    State state_;

    std::vector<Undo> history_;

private:
    // Used to calculate Zobrist hash key for a position.
    // InitZobristHashing() fills this with random numbers 
    // for each piece-square combination.
    static u64 zobrist_table[NUM_PIECE_TYPES][NUM_SQUARES];
};

#endif //BOARD_H_
