#pragma once
#include <vector>

#include <SFML/Graphics.hpp>

#include "Path.h"
#include "AnimatedSprite.h"

namespace tt
{

class Background;
using BackgroundSharedPtr = std::shared_ptr<Background>;

class Vehicle;
using VehiclePtr = std::shared_ptr<Vehicle>;

class Vehicle : public AnimatedSprite
{
    sf::Clock           _movementClock;
    BackgroundSharedPtr _background;

    Path                _path;
    sf::Vector2i        _lastPathPoint;
    const sf::Vector2i  _tilesize;

public:
    Vehicle(sf::Texture texture, const sf::Vector2i& size, BackgroundSharedPtr bg);

    std::uint16_t timestep() override;

private:
    void move();

};

class VehicleFactory
{

};

} // namespace tt