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

class VehicleFactory
{
    BackgroundSharedPtr             _background;
    ResourceManager&                _resources;
    std::shared_ptr<PathFactory>    _pathFactory;
    nl::json                        _json;

public:
    VehicleFactory(ResourceManager& resmgr, BackgroundSharedPtr bg);

    void setPathFactory(PathFactoryPtr pf) { _pathFactory = pf; }
    PathFactoryPtr pathFactory() { return _pathFactory; }

    VehiclePtr createVehicle();
};

} // namespace tt