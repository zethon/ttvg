#pragma once
#include <memory>

#include <nlohmann/json.hpp>

#include "Path.hpp"
#include "ResourceManager.h"
#include "Intersection.h"

namespace nl = nlohmann;

namespace tt
{

class PathFactory;
using PathFactoryPtr = std::shared_ptr<PathFactory>;

class Vehicle;
using VehiclePtr = std::shared_ptr<Vehicle>;

struct VehicleInfo
{
    sf::Texture*    texture = nullptr;
    sf::Vector2f    size;   
    sf::Vector2f    scale;
    sf::Vector2f    speed;  // the car's speed is randomly selected within this range
};

class VehicleFactory
{
    BackgroundSharedPtr             _background;
    ResourceManager&                _resources;
    std::shared_ptr<PathFactory>    _pathFactory;
    std::vector<VehicleInfo>        _vehicles;

public:
    VehicleFactory(ResourceManager& resmgr, BackgroundSharedPtr bg);

    void setPathFactory(PathFactoryPtr pf) { _pathFactory = pf; }
    PathFactoryPtr pathFactory() { return _pathFactory; }

    VehiclePtr createVehicle();

private:
    void loadVehicles(const nl::json& json);
};

} // namespace tt