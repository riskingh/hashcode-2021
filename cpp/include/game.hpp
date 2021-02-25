#ifndef GAME_H
#define GAME_H

#include <istream>
#include <string>
#include <vector>

struct Game;
struct Street;
struct Car;
struct Intersection;

struct Game {
    int D;
    int I;
    int S;
    int V;
    int F;

    std::vector<Street> streets;
    std::vector<std::string> external_street_mapping;

    std::vector<Car> cars;

    std::vector<Intersection> intersections;
};

struct Street {
    int id;
    int begin;
    int end;
    int L;
};

struct Car {
    std::vector<int> path_streets;
};

struct Intersection {
    int id;
    std::vector<int> in_streets;
    std::vector<int> out_streets;  
};

struct GameSolution {
    std::vector<std::vector<std::pair<int, int>>> intersection_assignment;
};

Game read_game(std::istream&);
void write_game_solution(std::ostream&, const Game&, const GameSolution&);

#endif // GAME_H
