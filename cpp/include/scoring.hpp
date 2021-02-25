#ifndef SCORING_H
#define SCORING_H

#include "game.hpp"
#include "solution.hpp"
#include <cstdint>

#include <queue>
#include <algorithm>
#include <iostream>


struct StreetImpl {
    const Street& data_;

    explicit StreetImpl(const Street& data)
        : data_(data) 
    {
    }

    void push(int v_idx) {
        queue.push(v_idx);
    }

    int pop() {
        if (empty()) {
            throw "EMPTY";
        }
        auto res = queue.front();
        queue.pop();
        return res;
    }

    bool empty() {
        return queue.empty();
    }

    std::queue<int> queue;
};


struct IntersectionImpl {

    const Intersection& data_;
    std::vector<std::pair<int, int>> schedule;
    std::vector<int64_t> prefix_sum;
    int64_t period = 0;

    explicit IntersectionImpl(const Intersection& data)
        : data_(data) 
    {
    }


    void set_schedule(const std::vector<std::pair<int, int>>& schedule_in) {
        schedule = schedule_in;
        prefix_sum.push_back(0);
        for (const auto& element : schedule) {
            prefix_sum.push_back(prefix_sum.back() + element.second);
        }
        period = prefix_sum.back();
    }

    int get_current_street(int64_t timestamp) {
        if (schedule.empty()) {
            return -1;
        }
        if (period == 0) {
            return -1;
        }
        int64_t reminder = timestamp % period;
        auto schedule_idx = std::upper_bound(prefix_sum.begin(), prefix_sum.end(), reminder) - prefix_sum.begin() - 1;
        return schedule[schedule_idx].first;
    }
};

struct CarImpl {
    const Car& data_;
    int street_rel_idx = 0;

    explicit CarImpl(const Car& data)
        : data_(data)
    {
    }

    void move() {
        ++street_rel_idx;
    }

    int get_current_street() {
        if (!(0 <= street_rel_idx && street_rel_idx < data_.path_streets.size())) {
            throw "Error";
        }
        return data_.path_streets[street_rel_idx];
    }

    bool is_finished() {
        return street_rel_idx + 1 == data_.path_streets.size();
    }
};


inline int64_t Score(const Game& input, const GameSolution& solution) {
    std::vector<std::vector<std::pair<int, int>>> car_events(input.D + 1);
    int64_t total_score = 0;
    std::vector<IntersectionImpl> intersections;
    std::vector<CarImpl> cars;
    std::vector<StreetImpl> streets;

    for (const auto& data : input.intersections) {
        intersections.emplace_back(data);
    }

    for (const auto& data : input.streets) {
        streets.emplace_back(data);
    }

    int idx = 0;
    for (const auto& data : input.cars) {
        cars.emplace_back(data);
        const auto& car = cars.back();
        streets[car.data_.path_streets[0]].push(idx);
        ++idx;
    }

    idx = 0;
    for (const auto& schedule : solution.intersection_assignment) {
        intersections[idx].set_schedule(schedule);
        ++idx;
    }

    for (int64_t timestamp = 0; timestamp <= input.D; ++timestamp) {
        for (const auto& event : car_events[timestamp]) {
            auto car_idx = event.first;
            auto street_idx = event.second;
            if (cars[car_idx].is_finished()) {
                total_score += input.F + (input.D - timestamp);
            } else {
                streets[street_idx].push(car_idx);
            }
        }

        for (int intersection_idx = 0; intersection_idx < intersections.size(); ++intersection_idx) {
            auto& intersection = intersections[intersection_idx];
            auto street_idx = intersection.get_current_street(timestamp);
            if (street_idx == -1) {
                continue;
            }
            if (!streets[street_idx].empty()) {
                auto car_idx = streets[street_idx].pop();
                cars[car_idx].move();
                auto next_street_idx = cars[car_idx].get_current_street();
                auto length = streets[next_street_idx].data_.L;
                if (timestamp + length <= input.D) {
                    car_events[timestamp + length].push_back({car_idx, next_street_idx});
                }
            }
        }
    }

    return total_score;
}

#endif