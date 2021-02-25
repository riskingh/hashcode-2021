#include "game.hpp"
#include "solution.hpp"

#include<utility>

class FirstSolution : public ISolution {

public:

    GameSolution solve(const Game& game) override {
        std::vector<std::vector<std::pair<int, int>>> out(game.I);

        for (int i = 0; i != game.I; ++i) {
            const auto& in_streets = game.intersections[i].in_streets;
            
            for (int street_id : in_streets) {
                out[i].emplace_back(street_id, 1);
            }
        }

        return GameSolution{out};
    }
};

std::unique_ptr<ISolution> MakeFirstSolution() {
    return std::make_unique<FirstSolution>();
}
