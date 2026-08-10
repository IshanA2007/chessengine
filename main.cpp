#include <iostream>
#include <sstream>

#include "board.h"
#include "bitboard.h"
#include "attacks.h"
#include "movegen.h"
#include "uci.h"

int main() {
    // UCI talks over a pipe; unbuffered stdout keeps the GUI from waiting on us
    std::cout << std::unitbuf;

    init_magics();

    Board board{};
    board.set_from_fen(START_FEN);

    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "uci")      { std::cout << "id name FishBot\nid author Ishan\nuciok\n"; }
        else if (cmd == "isready")  { std::cout << "readyok\n"; }
        else if (cmd == "ucinewgame") { board.set_from_fen(START_FEN); }
        else if (cmd == "position") { handle_position(board, ss); }
        else if (cmd == "go")       { handle_go(board, ss); }
        else if (cmd == "quit")     { break; }
        //non uci
        else if (cmd == "perft") { handle_perft(board);}
    }
    return 0;
}
