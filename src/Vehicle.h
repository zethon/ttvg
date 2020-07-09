#pragma once
#include <vector>

#include <SFML/Graphics.hpp>

#include "Path.h"
#include "AnimatedSprite.h"

namespace tt
{

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Background;
using BackgroundSharedPtr = std::shared_ptr<Background>;

class Vehicle;
using VehiclePtr = std::shared_ptr<Vehicle>;

class Vehicle : public AnimatedSprite
{

public:
    enum TimeStep
    {
        NOOP = 0,
        DELETE = 1
    };

    enum State
    {
        MOVING,
        STOPPED
    };

    Vehicle(sf::Texture texture, const sf::Vector2i& size, BackgroundSharedPtr bg);

    std::uint16_t timestep() override;
    bool isBlocked(const sf::FloatRect& point);

    State vehicleState() const { return _state; }
    void setVehicleState(State val) { _state = val; }

private:
    void move();

    sf::Clock           _movementClock;
    sf::Clock           _lifeClock;

    BackgroundSharedPtr _background;

    Path                _path;
    sf::Vector2i        _lastPathPoint;
    const sf::Vector2i  _tilesize;

    Direction           _direction = DOWN;
    State               _state = MOVING;

};

bool isPathBlocked(const sf::FloatRect& object, const sf::FloatRect& other,
    Vehicle::Direction direction, float minDistance);

} // namespace tt