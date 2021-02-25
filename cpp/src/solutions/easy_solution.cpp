#include "game.hpp"
#include "solution.hpp"

#include <algorithm>
#include <utility>
#include <cmath>

const int APPROX_INTERSECTION_TURNAROUND = 20;

class FirstSolution : public ISolution {

public:

    GameSolution solve(const Game& game) override {
        std::vector<int> street_car_count(game.S);

        for (const auto& car : game.cars) {
            for (const auto& street_id : car.path_streets) {
                ++street_car_count[street_id];
            }
        }

        std::vector<std::vector<std::pair<int, int>>> out(game.I);

        for (int i = 0; i != game.I; ++i) {
            const auto& in_streets = game.intersections[i].in_streets;

            std::vector<std::pair<int, int>> weighted_streets;
            for (const auto& street_id : in_streets) {
                weighted_streets.emplace_back(street_car_count[street_id], street_id);
            }
            std::sort(weighted_streets.rbegin(), weighted_streets.rend());
            int sum = 0;
            for (const auto& [weight, _] : weighted_streets) {
                sum += weight;
            }
            
            for (const auto& [weight, street_id] : weighted_streets) {
                int t = std::max((int)round((double)weight * APPROX_INTERSECTION_TURNAROUND / sum), 1);
                out[i].emplace_back(street_id, t);
            }
        }

        return GameSolution{out};
    }
};

std::unique_ptr<ISolution> MakeFirstSolution() {
    return std::make_unique<FirstSolution>();
}
