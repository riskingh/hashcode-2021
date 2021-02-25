#include "game.hpp"
#include "solution.hpp"

#include <algorithm>
#include <utility>
#include <cmath>
#include <random>
#include "scoring.hpp"

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

const int APPROX_INTERSECTION_TURNAROUND = 20;
const auto GOOD_APPROX = {1, 2, 3, 6, 23};

class FirstSolution2 : public ISolution {

public:

    GameSolution solve(const Game& game) override {
        GameSolution sol, best_sol;
        int64_t score = 0, best_score = 0, best_approx_sol = -1;

        for (auto approx_smth : GOOD_APPROX) {
            int max_seed = 30;
            boost::asio::thread_pool pool(max_seed);
            std::vector<GameSolution> solutions(max_seed);
            std::vector<int64_t> scores(max_seed);

            for (int seed = 0; seed < max_seed; ++seed) {
                boost::asio::post(pool, [this, approx_smth, seed, &solutions, &scores, &game] {
                    solutions[seed] = solve_once(game, approx_smth, seed);
                    scores[seed] = Score(game, solutions[seed], nullptr);
                });
            }
            pool.join();

            for (int seed = 0; seed < max_seed; ++seed) {
                if (scores[seed] > best_score) {
                    best_sol = solutions[seed];
                    best_score = scores[seed];
                }
            }
        }

        return best_sol;
    }

    GameSolution solve_once(const Game& game, int approx_smth, int seed) {
        auto rng = std::default_random_engine{static_cast<unsigned int>(seed)};
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
