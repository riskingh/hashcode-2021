#include "game.hpp"
#include "solution.hpp"
#include "scoring.hpp"

#include <algorithm>
#include <utility>
#include <cmath>
#include <random>


#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>


namespace {
    class FirstSolution : public ISolution {

    public:
        FirstSolution(int seed, int APPROX_INTERSECTION_TURNAROUND) {
            seed_ = seed;
            APPROX_INTERSECTION_TURNAROUND_ = APPROX_INTERSECTION_TURNAROUND;
        }

        GameSolution solve(const Game& game) override {
            auto rng = std::default_random_engine{static_cast<unsigned int>(seed_)};
            std::uniform_int_distribution<> distrib(std::max(1, APPROX_INTERSECTION_TURNAROUND_ - 1), APPROX_INTERSECTION_TURNAROUND_ + 1);

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
                
                // auto rng = std::default_random_engine{static_cast<unsigned int>(seed_)};
                // std::shuffle(weighted_streets.begin(), weighted_streets.end(), rng);

                int sum = 0;
                for (const auto& [weight, _] : weighted_streets) {
                    sum += weight;
                }
                
                for (const auto& [weight, street_id] : weighted_streets) {
                    int x = distrib(rng);
                    int t = std::max((int)round((double)weight * x / sum), 1);
                    out[i].emplace_back(street_id, t);
                }
            }

            return GameSolution{out};
        }

        int seed_ = 0;
        int APPROX_INTERSECTION_TURNAROUND_ = 20;
    };
}

class SuperRandomSolution : public ISolution {
public:
    GameSolution solve(const Game& game) override {
        GameSolution bestSolution;
        int64_t bestScore = -1;
        for (int j = 1; j < 20; ++j) {
            std::vector<GameSolution> solutions(20);
            std::vector<int64_t> scores(20);
            boost::asio::thread_pool pool(20);

            for (int i = 0; i < 20; ++i) {
                boost::asio::post(pool, [i, j, &solutions, &scores, &game] {
                    solutions[i] = FirstSolution(i, j).solve(game);
                    scores[i] = Score(game, solutions[i], nullptr);
                });
            }
            pool.join();

            for (int i = 0; i < 20; ++i) {
                if (scores[i] > bestScore) {
                    bestScore = scores[i];
                    bestSolution = std::move(solutions[i]);
                }
            }
            std::cerr << j << " " << bestScore << std::endl;
        }

        return bestSolution;
    }

};




std::unique_ptr<ISolution> MakeSuperRandomSolution() {
    return std::make_unique<SuperRandomSolution>();
}
