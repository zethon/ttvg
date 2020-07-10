#pragma once
#include <memory>

#include <nlohmann/json.hpp>

#include "Path.h"
#include "ResourceManager.h"

namespace nl = nlohmann;

namespace tt
{

class Vehicle;
using VehiclePtr = std::shared_ptr<Vehicle>;

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