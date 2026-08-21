#include <iostream>
#include <sstream>

#include "board.h"
#include "attacks.h"
#include "movegen.h"
#include "search.h"
#include "uci.h"
#include "zobrist.h"
#include "transposition.h"

int main() {
    // UCI talks over a pipe; unbuffered stdout keeps the GUI from waiting on us
    std::cout << std::unitbuf;

    init_magics();
    init_zobrist();
    tt_init();

    Board board{};
    board.set_from_fen(START_FEN);

    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "uci")      { std::cout << "id name FishBot\nid author Ishan\nuciok\n"; }
        else if (cmd == "isready")  { std::cout << "readyok\n"; }
        else if (cmd == "ucinewgame") { board.set_from_fen(START_FEN); history_reset(board.hash); tt_clear();}
        else if (cmd == "position") { handle_position(board, ss); }
        else if (cmd == "go")       { handle_go(board, ss); }
        else if (cmd == "quit")     { break; }
        //non uci
        else if (cmd == "perft") { handle_perft(board);}
        else if (cmd == "bench") { int d; if (!(ss >> d)) d = 10; handle_bench(d); }
    }
    return 0;
}
