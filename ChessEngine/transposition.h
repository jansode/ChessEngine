#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "types.h"
#include "move.h"

enum TTEntryType
{
	TTENTRY_EXACT,
	TTENTRY_UPPER,
	TTENTRY_LOWER
};

struct TTEntry
{
	u64 hash;
	int evaluation;
	TTEntryType type;
	Move best_move;
};

class TranspositionTable
{
	TTEntry *entries;

	unsigned size_mb;
	unsigned num_entries;
	unsigned max_entries;

public:
	TranspositionTable(unsigned size_mb);
	void SetSize(unsigned size_mb);
	unsigned NumEntries() const {return num_entries;}
};

#endif
