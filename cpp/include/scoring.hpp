#ifndef SCORING_H
#define SCORING_H

#include "game.hpp"
#include "solution.hpp"
#include <cstdint>

#include <queue>
#include <algorithm>
#include <iostream>



struct DebugInfo {
    struct Event {
        int car_idx;
        int street_idx;
        int intersection_idx;
        int64_t arrive_timestamp;
        int64_t wait_time;
        bool is_first = false;
    };

    int cars_not_finished = 0;

    std::vector<Event> events;


    void Print();
};

int64_t Score(const Game& input, const GameSolution& solution, DebugInfo* debug_info_sink);

#endif
