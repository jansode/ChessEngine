#include "transposition.h"

const int kDefaultTTSize = 64;

TranspositionTable::TranspositionTable(unsigned size_mb = kDefaultTTSize)
{
	this->size_mb = size_mb;
	SetSize(size_mb);
}

void TranspositionTable::SetSize(unsigned size_mb)
{
}

