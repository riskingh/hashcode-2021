#include "game.hpp"
#include "solution.hpp"

#include <algorithm>
#include <utility>
#include <cmath>
#include <iostream>

const int APPROX_INTERSECTION_TURNAROUND = 3;

class FirstSolution : public ISolution {

public:

    GameSolution solve(const Game& game) override {
        std::vector<int> street_car_count(game.S);

        std::vector<long long> cars_way_length;
        for (const auto& car : game.cars) {
            int car_way_length = 0;
            for (const auto& street_id : car.path_streets) {
                car_way_length += game.streets[street_id].L;
            }
            cars_way_length.push_back(car_way_length);
        }
        std::sort(cars_way_length.begin(), cars_way_length.end());
        long long threshold = cars_way_length[(cars_way_length.size() * 0 / 100)];

        std::vector<std::vector<int> > cars_in_same_time(game.S, std::vector<int>(std::max(game.D, 10000), 0));

        for (const auto& car : game.cars) {
            int car_way_length = 0;
            for (const auto& street_id : car.path_streets) {
                car_way_length += game.streets[street_id].L;
            }
            if (car_way_length < threshold) {
                continue;
            }
            
            int path_length = 0;
            for (const auto& street_id : car.path_streets) {
                path_length += game.streets[street_id].L;
                cars_in_same_time[street_id][path_length % 10] += 1;
                // street_car_count[street_id] += path_number;
            }
        }

        for (const auto& street : game.streets) {
            int max_cars_same_time = 0;
            for (const auto& cars_same_time : cars_in_same_time[street.id]) {
                if (cars_same_time > max_cars_same_time) {
                    max_cars_same_time = cars_same_time;
                }
            }
            street_car_count[street.id] = max_cars_same_time;
        }

        // for (const auto& i : street_car_count) {
        //     std::cout << i << ' ';
        // }

        std::vector<std::vector<std::pair<int, int>>> out(game.I);

        for (int i = 0; i != game.I; ++i) {
            const auto& in_streets = game.intersections[i].in_streets;

            std::vector<std::pair<int, int>> weighted_streets;
            for (const auto& street_id : in_streets) {
                weighted_streets.emplace_back(street_car_count[street_id] / 2, street_id);
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
