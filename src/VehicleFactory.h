#pragma once
#include <memory>

#include <nlohmann/json.hpp>

#include <SFML/Audio.hpp>

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
    sf::Texture*        texture = nullptr;
    sf::SoundBuffer*    sound = nullptr;
    sf::Vector2f        size;   
    sf::Vector2f        scale;
    sf::Vector2f        speed;  // the car's speed is randomly selected within this range
};

class VehicleFactory
{
    BackgroundSharedPtr             _background;
    ResourceManager&                _resources;
    std::shared_ptr<PathFactory>    _pathFactory;
    std::vector<VehicleInfo>        _vehicles;
    bool                            _highlighted = false;

public:
    VehicleFactory(ResourceManager& resmgr, BackgroundSharedPtr bg);

    void setPathFactory(PathFactoryPtr pf) { _pathFactory = pf; }
    PathFactoryPtr pathFactory() { return _pathFactory; }

    void setHighlighted(bool b) { _highlighted = b; }
    bool highlighted() const { return _highlighted; }

    VehiclePtr createVehicle();

private:
    void loadVehicles(const nl::json& json);
};

} // namespace tt