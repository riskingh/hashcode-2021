#include "game.hpp"
#include "solution.hpp"
#include "scoring.hpp"

#include <algorithm>
#include <utility>
#include <cmath>
#include <map>
#include <random>
#include <set>
#include <future>

const int STEP_NUM = 10;
const int SOLUTION_PICK_NUM = 90;

struct Modification {
    int intersection_idx;
    int street_idx;
    int delta;
};

struct LocalSolution {
    int score;
    DebugInfo debug_info;
    std::vector<Modification> solution;
};

class IterationSolution : public ISolution {

public:

    GameSolution solve(const Game& game) override {
        auto bool_dist = std::uniform_int_distribution<>(0, 1);

        auto solution = MakeFirstSolution2()->solve(game);

        std::map<int, std::map<int, int>> solution_intersection_to_street_idx;
        for (int intersection_idx = 0; intersection_idx != solution.intersection_assignment.size(); ++intersection_idx) {
            for (int idx = 0; idx != solution.intersection_assignment[intersection_idx].size(); ++idx) {
                solution_intersection_to_street_idx[intersection_idx][solution.intersection_assignment[intersection_idx][idx].first] = idx;
            }
        }

        std::set<int> ignore_intersections;

        DebugInfo debug_info;
        auto score = Score(game, solution, &debug_info);

        for (size_t i = 0; i != STEP_NUM; ++i) {
            std::cerr << "score: " << score << '\n';

            std::vector<int64_t> intersection_wait_sum(game.I);
            for (const auto& event : debug_info.events) {
                intersection_wait_sum[event.intersection_idx] += event.wait_time;
            }

            std::vector<std::pair<int64_t, int>> intersection_weighted;
            for (int i = 0; i != game.I; ++i) {
                int w = ignore_intersections.find(i) == ignore_intersections.end() ? intersection_wait_sum[i] : -1;
                intersection_weighted.emplace_back(w, i);
            }
            sort(intersection_weighted.rbegin(), intersection_weighted.rend());

            std::map<int, std::map<int, int64_t>> intersection_to_street_wait_sum;
            for (const auto& event : debug_info.events) {
                intersection_to_street_wait_sum[event.intersection_idx][event.street_idx] += event.wait_time;
            }

            auto solve = [&](int seed) {
                auto rng = std::default_random_engine{static_cast<unsigned int>(seed)};
                std::vector<Modification> modifications;

                for (int i = 0; i != std::min(20, (int)intersection_weighted.size()); ++i) {
                    int intersection_idx = intersection_weighted[i].second;
                    if (bool_dist(rng)) continue;

                    std::vector<std::pair<int64_t, int>> weighted_streets;
                    for (const auto& [street_id, sum_weight_time] : intersection_to_street_wait_sum[intersection_idx]) {
                        weighted_streets.emplace_back(sum_weight_time, street_id);
                    }
                    std::sort(weighted_streets.rbegin(), weighted_streets.rend());

                    // ignore_intersections.insert(intersection_idx);
                    for (int j = 0; j != std::min(1, (int)weighted_streets.size()); ++j) {
                        int street_idx = weighted_streets[j].second;
                        modifications.emplace_back(Modification{intersection_idx, street_idx, std::uniform_int_distribution<>(1, 3)(rng)});
                    }
                }

                GameSolution new_solution = build_solution(solution, solution_intersection_to_street_idx, modifications);
                DebugInfo new_debug_info;
                auto new_score = Score(game, new_solution, &new_debug_info);
                return LocalSolution{new_score, new_debug_info, modifications};
            };

            std::vector<std::thread> threads;
            std::vector<std::future<LocalSolution>> futures;

            for (int i = 0; i != SOLUTION_PICK_NUM; ++i) {
                // threads.emplace_back(solve, i);
                futures.emplace_back(std::async(std::launch::async, solve, i));
            }
            for (auto &thread : futures) {
                thread.wait();
            }

            int best_score = -1, best_score_idx;
            std::vector<LocalSolution> solutions;

            for (int i = 0; i != SOLUTION_PICK_NUM; ++i) {
                solutions.emplace_back(futures[i].get());
                if (solutions[i].score > best_score) {
                    best_score = solutions[i].score;
                    best_score_idx = i;
                }
            }

            score = solutions[best_score_idx].score;
            debug_info = solutions[best_score_idx].debug_info;
            solution = build_solution(solution, solution_intersection_to_street_idx, solutions[best_score_idx].solution);

            for (const auto& modification : solutions[best_score_idx].solution) {
                ignore_intersections.insert(modification.intersection_idx);
            }
        }

        return solution;
    }

    GameSolution build_solution(
        GameSolution solution,
        const std::map<int, std::map<int, int>>& solution_intersection_to_street_idx,
        const std::vector<Modification>& modifications) {
            for (const auto& modification : modifications) {
                solution.intersection_assignment[modification.intersection_idx][solution_intersection_to_street_idx.at(modification.intersection_idx).at(modification.street_idx)].second += modification.delta;
            }
            return solution;
        }
};

std::unique_ptr<ISolution> MakeIterationSolution() {
    return std::make_unique<IterationSolution>();
}
