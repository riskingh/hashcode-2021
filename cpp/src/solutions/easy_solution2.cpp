#include "game.hpp"
#include "solution.hpp"

#include <algorithm>
#include <utility>
#include <cmath>
#include <random>
#include "scoring.hpp"

const int APPROX_INTERSECTION_TURNAROUND = 20;
const auto GOOD_APPROX = {1, 2, 3, 6, 23};
auto rng = std::default_random_engine{static_cast<unsigned int>(0)};

class FirstSolution2 : public ISolution {

public:

    GameSolution solve(const Game& game) override {
        GameSolution sol, best_sol;
        int64_t score = 0, best_score = 0, best_approx_sol = -1;

        for (auto i : GOOD_APPROX) {
            for (int seed = 1; seed < 10; ++seed) {
                rng = std::default_random_engine{static_cast<unsigned int>(seed)};
                DebugInfo debug_info;
                sol = solve_once(game, i);
                score = Score(game, sol, &debug_info);
                if (score > best_score) {
                    best_sol = sol;
                    best_score = score;
                    best_approx_sol = i;
                }
            }
        }
        std::cerr << "Found solution with approx_smth = " << best_approx_sol << std::endl;

        return best_sol;
    }

    GameSolution solve_once(const Game& game, int approx_smth) {
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
            /* std::sort(weighted_streets.rbegin(), weighted_streets.rend()); */
            shuffle(weighted_streets.begin(), weighted_streets.end(), rng);
            int sum = 0;
            for (const auto& [weight, _] : weighted_streets) {
                sum += weight;
            }
            
            for (const auto& [weight, street_id] : weighted_streets) {
                int t = std::max((int)round((double)weight * approx_smth / sum), 1);
                out[i].emplace_back(street_id, t);
            }
        }

        return GameSolution{out};
    }
};

std::unique_ptr<ISolution> MakeFirstSolution2() {
    return std::make_unique<FirstSolution2>();
}
