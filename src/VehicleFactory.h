#pragma once
#include <memory>

#include <nlohmann/json.hpp>

#include "Path.h"
#include "ResourceManager.h"
#include "Intersection.h"

namespace nl = nlohmann;

namespace tt
{

class Vehicle;
using VehiclePtr = std::shared_ptr<Vehicle>;

class PathFactory
{
    Intersections   _edges;
    Intersections   _turns;
    sf::Vector2i    _size;

public:
    PathFactory(const sf::Vector2i& size)
        : _size{ size }
    {}

    void setEdges(const Intersections& edges) { _edges = edges; }
    void setIntersections(const Intersections& points) { _turns = points; }
    
    Path makeRandomPath() const;

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