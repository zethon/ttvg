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
    TurningPoints   _edges;
    TurningPoints   _turns;
    sf::Vector2i    _size;

public:
    PathFactory(const sf::Vector2i& size)
        : _size{ size }
    {}

    void setEdges(const TurningPoints& edges) { _edges = edges; }
    void setIntersections(const TurningPoints& points) { _turns = points; }
    
    Path makeRandomPath() const;
};

class VehicleFactory
{
    BackgroundSharedPtr _background;
    ResourceManager&    _resources;
    nl::json            _json;

public:
    VehicleFactory(ResourceManager& resmgr, BackgroundSharedPtr bg);


    VehiclePtr createVehicle();
};

} // namespace tt