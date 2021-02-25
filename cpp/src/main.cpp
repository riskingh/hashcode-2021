#include "game.hpp"
#include "solution.hpp"
#include "scoring.hpp"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    assert (argc == 3 || argc == 4 || argc == 5);
    std::ifstream istream(argv[1]);
    std::ostream* ostream = &std::cout;
    std::unique_ptr<std::ofstream> ofstream;
    if (strcmp(argv[2], "-") != 0) {
        ofstream = std::make_unique<std::ofstream>(argv[2]);
        ostream = ofstream.get();
    }

    std::ofstream output_stream(argv[2]);
    std::string solution_name = "";

    if (argc == 4) {
        solution_name = std::string{argv[3]};
    }

    auto solution = get_solution(solution_name);
    auto input = read_game(istream);
    auto output = solution->solve(input);

    write_game_solution(*ostream, input, output);
    DebugInfo debug_info;
    auto score = Score(input, output, &debug_info);

    std::cerr << argv[1] << " score is " << score << std::endl;

    // debug_info.Print();
    return 0;
}
