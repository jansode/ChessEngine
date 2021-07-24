#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <iomanip>

#include "uci.h"
#include "move_generation.h"
#include "search.h"
#include "util.h"
#include "tests.h"
#include "bitboards.h"


namespace 
{
	void tokenize_command(std::vector<std::string>* tokens, std::istringstream *is)
	{
		std::string token;
		while (*is >> token) tokens->push_back(token);
	}

    const std::string kFenStartPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	void position(Board *board, const std::vector<std::string> &tokens)
	{
		if (tokens.size() == 0)
		{
			std::cout << "Missing arguments\n";
			return;
		}

		int curr_token = 0;
		if (tokens[curr_token] == "fen")
		{
			std::string fen;
			while (++curr_token < tokens.size() && tokens[curr_token] != "moves")
			{
				// Manually add the spaces to the fen string.
				// We got rid of them when tokenizing the input.
				fen += tokens[curr_token] + ' ';
			}

			board->SetPositionFromFEN(fen);
		}
		else if (tokens[curr_token] == "startpos")
		{
			board->SetPositionFromFEN(kFenStartPosition);
			++curr_token;
		}
		else return;

        if(curr_token < tokens.size() && tokens[curr_token] == "moves")
        {
			while (++curr_token < tokens.size())
			{
                Move m = move_from_uci(*board,tokens[curr_token]);

                if(m.type != MOVE_TYPE_NONE)
                {
                    board->MakeMove(m);
                }
				else
				{
					board->Reset();
					std::cout << "Illegal moves in move list." << std::endl;
				}
			}
        }
    }

    void print_help()
    {
        std::cout<<"uci\n\tTell engine to use the uci protocol.\n"<<
                   "debug [ on | off]\n\tSwitch the debug mode on and off.\n"<<
                   "isready\n\tUsed to synchronize the engine with the GUI. \n"<<
                   "setoption name [value]\n\tSent to the engine when the user wants to change the internal parameters of the engine.\n"<<
                   "ucinewgame\n\tThis is sent to the engine when the next search will be from a different game.\n"<<
                   "position [fen | startpos] moves ...\n\tSet up the position described in fenstring on the internal board and play the moves.\n"<<
                   "go\n\tStart calculating on the current position set up with the position command.\n"<<
                   "stop\n\tStop calculating as soon as possible.\n"<<
                   "ponderhit\n\tThe used has played the expected move.\n"<<
                   "perft [fen] [depth]\n"<<
                   "quit\n\tQuit the program as soon as possible\n"<<std::endl;
    }

    void print_legal_moves(Board *board, const std::vector<std::string>& tokens) 
    {
		std::string token = "all";

		if (tokens.size() != 0)
			token = tokens[0];

        std::vector<Move> move_list;
        if(token == "pawns")
        {
            if(board->SideToMove() == WHITE)
                move_generation::PseudoLegalPawns<WHITE>(*board,&move_list);
            else
                move_generation::PseudoLegalPawns<BLACK>(*board,&move_list);
        }
        else if(token == "bishops")
        {
            if(board->SideToMove() == WHITE)
                move_generation::PseudoLegalBishops<WHITE>(*board,&move_list);
            else
                move_generation::PseudoLegalBishops<BLACK>(*board,&move_list);
        }
        else if(token == "knights")
        {
            if(board->SideToMove() == WHITE)
                move_generation::PseudoLegalKnights<WHITE>(*board,&move_list);
            else
                move_generation::PseudoLegalKnights<BLACK>(*board,&move_list);

        }
        else if(token == "rooks")
        {
            if(board->SideToMove() == WHITE)
                move_generation::PseudoLegalRooks<WHITE>(*board,&move_list);
            else
                move_generation::PseudoLegalRooks<BLACK>(*board,&move_list);
        }
        else if(token == "queens")
        {
            if(board->SideToMove() == WHITE)
                move_generation::PseudoLegalQueens<WHITE>(*board,&move_list);
            else
                move_generation::PseudoLegalQueens<BLACK>(*board,&move_list);
        }
        else if(token == "kings")
        {
            if(board->SideToMove() == WHITE)
                move_generation::PseudoLegalKings<WHITE>(*board,&move_list);
            else
                move_generation::PseudoLegalKings<BLACK>(*board,&move_list);
        }
        else if(token == "all")
        {
            if(board->SideToMove() == WHITE)
                move_generation::LegalAll<WHITE>(*board,&move_list);
            else
                move_generation::LegalAll<BLACK>(*board,&move_list);
        }
        else
        {
            std::cout<<"Unkown command: legal "<<token<<std::endl;
            return;
        }

        std::cout<<"Number of legal moves: "<<move_list.size()<<std::endl;
        for(int i=0;i<move_list.size();++i)
        {
            std::string from = algebraic_from_square(move_list[i].from);
            std::string to = algebraic_from_square(move_list[i].to);
            std::cout<< get_piece_name(move_list[i].piece) << " " << from<<" "<<to;

			if (move_list[i].capture)
			{
				std::cout << " capture\n";
				continue;
			}

			std::cout << " " << get_move_type(move_list[i].type);
			if (move_list[i].type == PROMOTION)
				std::cout << " to " << get_piece_name(move_list[i].promotion);

			std::cout << std::endl;

			
        }
    }

    void go(Search *search, Board *board, const std::vector<std::string>& tokens)
    {
		for (int i=0; i<tokens.size(); ++i)
		{
			if (tokens[i] == "infinite")
			{
				search->duration = 0;
			}
			else if (tokens[i] == "movetime")
			{
                if(i == tokens.size()-1)
                {
                    std::cout<<"Missing parameter to "<<tokens[i]<<"\n";
                    return;
                }

				search->duration = std::stoi(tokens[++i],nullptr,10);
			}
            else if (tokens[i] == "nodes")
            {
                if(i == tokens.size()-1)
                {
                    std::cout<<"Missing parameter to "<<tokens[i]<<"\n";
                    return;
                }
				search->nodes = std::stoi(tokens[++i],nullptr,10);

            }
            else if (tokens[i] == "depth")
            {
                if(i == tokens.size()-1)
                {
                    std::cout<<"Missing parameter to "<<tokens[i]<<"\n";
                    return;
                }
				search->depth = std::stoi(tokens[++i],nullptr,10);
            }
		}
    }

	void list_attacked(Board *board)
	{
		for (int square = A8; square < NUM_SQUARES; ++square)
		{
			if (board->SquareAttacked((Square)square, board->SideToMove()))
			{
				std::cout << algebraic_from_square((Square)square) << "\n";
			}
		}
	}

	void make_move(Board *board, const std::vector<std::string>& tokens)
	{
		if (tokens.size() < 1) return;

		Move move = move_from_uci(*board,tokens[0]);
		board->MakeMove(move);
	}

    void attacks(const std::vector<std::string>& tokens)
    {
        if(tokens.size() < 1)
        {
            std::cout<<"attacks <piece>\n";
            return;
        }

        std::string piece_str = "white " + tokens[0];
        PieceType piece_type = piece_type_from_string(piece_str);

        if(piece_type == PIECE_TYPE_NONE)
        {
            std::cout<<tokens[0]<<" doesn't have a corresponding piece type.\n";
            return;
        }

        tests::print_attacks(piece_type_from_string(piece_str));
    }

    void perft(Board *board, const std::vector<std::string>& tokens)
    {
        if (tokens.size() == 0)
        {
            tests::start_perft(kDefaultPerftDepth,false);
            return;
        }
        else if(tokens[0] == "current")
        {
            unsigned depth = kDefaultPerftDepth;
            
            if(tokens.size() == 2)
                depth = std::stoi(tokens[1]);

            auto found = tests::PerftTestPositions.find(board->GenerateFenString());
            if(found != tests::PerftTestPositions.end())
                tests::perft_results(*board, &found->second, depth, false);
            else
                tests::perft_results(*board, nullptr, depth, false);
        }
        else if(tokens[0] == "list")
        {
            for(auto result : tests::PerftTestPositions)
            {
                std::cout<<"Fen: "<<result.second.fen<<"\n";
                board->SetPositionFromFEN(result.second.fen);
                board->PrintPosition();
            }
        }
    }

    void piece_bitboard(Board *board, const std::vector<std::string>& tokens)
    {
        if(tokens.size() == 1)
        {
            if(tokens[0] == "occupied")
            {
                print_bitboard(board->GetOccupied());
                return;
            }

        }

        if(tokens.size() < 2)
            return;

        std::string piece_type = std::string("") + tokens[0] + " " + tokens[1];
        PieceType piece = piece_type_from_string(piece_type);
        if(piece == PIECE_TYPE_NONE)
            return;

        print_bitboard(board->pieces_[piece]);
    }
};

namespace uci
{
    void loop()
    {
        Board board;
        Search search;

		bool search_active = false;

        std::string line, command;
        for(;;)
        {
			std::cout << "<<";
            getline(std::cin,line);

            std::istringstream iss(line);
            iss >> std::skipws >> command;

			std::vector<std::string> tokens;
			tokenize_command(&tokens, &iss);

            if(command == "uci")
            {
                std::cout << "id name chess engine\n"
                          << "id author Jan S\n"
                          << "uciok\n";
            }
            else if(command == "position")
            {
                position(&board, tokens);
            }
            else if(command == "go")
            {
				//if(search_active) continue;

		        std::thread search_thread;

                go(&search, &board, tokens);
				search_thread = std::thread(StartSearch,&search,&board);
                search_thread.detach();
				search_active = true;
            }
            else if(command == "stop")
            {
				if(!search_active) continue;

                TerminateSearch(&search,true);
				search_active = false;
            }
            else if(command == "print")
            {
                board.PrintPosition();
            }
            else if(command == "help")
            {
                print_help();
            }
            else if(command == "legal")
            {
                print_legal_moves(&board, tokens);
            }
            else if(command == "state")
            {
                board.PrintState();
            }
			else if(command == "perft")
			{
                perft(&board, tokens);
			}
			else if (command == "attacks")
			{
                attacks(tokens);
			}
			else if (command == "attacked")
			{
				// Lists the currenty attacked squared 
				// for the opposing side.
				list_attacked(&board);
			}
            else if (command == "bitboard")
            {
                piece_bitboard(&board,tokens);
            }
			else if (command == "makemove")
			{
				make_move(&board, tokens);
			}
            else if (command == "undomove")
            {
                board.UndoMove();
            }
			else if (command == "reset")
			{
				board.Reset();
			}
			else if (command == "ucinewgame")
			{
				// TODO
			}
			else if (command == "fen")
			{
				std::cout << board.GenerateFenString() << "\n";
			}
			else if (command == "evaluate")
			{
				float evaluation = board.Evaluate();
				std::cout << "Evaluation: " << std::fixed << std::setw(3) << std::setprecision(2) << evaluation << "\n";
			}
            else if(command == "isready")
            {
                std::cout<< "readyok\n"; 
            }
            else if(command == "quit")
            {
                exit(EXIT_SUCCESS);
            }
            else
            {
                if(std::cin.eof())
                {
                    exit(EXIT_SUCCESS);
                }

                std::cout<<"Unknown command: "<< command << std::endl;
            }
        }
    }
}



