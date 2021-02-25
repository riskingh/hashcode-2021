#ifndef SOLVE_H
#define SOLVE_H

#include "game.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <stdexcept>

class ISolution {
public:
    virtual ~ISolution() = default;
    virtual GameSolution solve(const Game& game) = 0;
};

std::unique_ptr<ISolution> MakeFirstSolution();
std::unique_ptr<ISolution> MakeFirstSolution2();
std::unique_ptr<ISolution> MakeIterationSolution();

inline std::unique_ptr<ISolution> get_solution(const std::string& name) {
    if (name == "easy") {
        return MakeFirstSolution();
    }
    if (name == "easy2") {
        return MakeFirstSolution2();
    }
    if (name == "iteration") {
        return MakeIterationSolution();
    }
    throw std::runtime_error(name);
}

#endif // SOLVE_H
