#pragma once
#include <memory>
#include  <random>
#include  <iterator>

#include <nlohmann/json.hpp>

#include "Path.h"
#include "ResourceManager.h"

namespace nl = nlohmann;

namespace tt
{

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) 
{
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) 
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

class Vehicle;
using VehiclePtr = std::shared_ptr<Vehicle>;

constexpr auto MAX_PATH_POINTS = 50;

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Intersection
{

public:
    enum Type
    {
        L0,
        L90,
        L180,
        L270,
        T0,
        T90,
        T180,
        T270
    }

    std::vector<Direction> getPossibleDirections(Direction sourceDirection) const
    {
        
    }

    Direction getRandomTurn(Direction sourceDirection) const;
    {
        const auto directions = getPossibleDirections();
        return select_randomly(directions);
    }

private:
    IntersectionType            _type;
    std::vector<Directions>     _directions;
};

class PathFactory
{

public:
    

};

class VehicleFactory
{
    ResourceManager&    _resources;
    nl::json            _json;

public:
    VehicleFactory(ResourceManager& resmgr, const std::string& mapname);


    VehiclePtr createVehicle();
};

} // namespace tt