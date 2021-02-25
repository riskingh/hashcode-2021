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

inline std::unique_ptr<ISolution> get_solution(const std::string& name) {
    if (name == "easy") {
        return MakeFirstSolution();
    }
    throw std::runtime_error(name);
}

#endif // SOLVE_H
