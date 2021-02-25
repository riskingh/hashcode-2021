#include "game.hpp"

#include <unordered_map>

Game read_game(std::istream& in) {
    Game game;
    in >> game.D >> game.I >> game.S >> game.V >> game.F;

    std::unordered_map<std::string, int> external_street_mapping;

    for (int i = 0; i != game.S; ++i) {
        Street street;
        std::string id;

        in >> street.begin >> street.end >> id >> street.L;
        street.id = i;

        game.streets.push_back(street);
        game.external_street_mapping.push_back(id);
        external_street_mapping[id] = street.id;
    }

    for (int i = 0; i != game.V; ++i) {
        Car car;
        int p;
        in >> p;

        for (int i = 0; i != p; ++i) {
            std::string street_id;
            in >> street_id;
            car.path_streets.push_back(external_street_mapping.at(street_id));
        }

        game.cars.push_back(car);
    }

    game.intersections.resize(game.I);
    for (int i = 0; i != game.I; ++i) {
        game.intersections[i].id = i;
    }
    for (const auto& street : game.streets) {
        game.intersections[street.begin].out_streets.push_back(street.id);
        game.intersections[street.end].in_streets.push_back(street.id);
    }

    return game;
}
