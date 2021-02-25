#include "game.hpp"

#include <fstream>
#include <iostream>

int main() {
    std::ifstream fin("../input/a.txt");

    Game game = read_game(fin);

    return 0;
}
