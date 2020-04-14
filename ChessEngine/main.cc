#include <iostream>
#include <vector>
#include <string>

#include "attacks.h"
#include "bitboards.h"
#include "board.h"
#include "move.h"
#include "move_generation.h"
#include "util.h"
#include "uci.h"
#include "tests.h"

int main(int argc, char **argv)
{
    Board::InitZobristHashing();
    move_generation::Init();
	tests::init_perft();
    uci::loop();
       
    return 0;
};
