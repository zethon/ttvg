#pragma once
#include <vector>

#include <SFML/Graphics.hpp>

#include "GameTypes.h"
#include "Path.hpp"
#include "AnimatedSprite.h"
#include "Intersection.h"

namespace tt
{

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

    void setPath(const Path& path);
    const Path& path() const { return _path; }
    Path& path() { return const_cast<Path&>((static_cast<const Vehicle&>(*this)).path()); }

private:
    void move();

    sf::Clock           _movementClock;
    sf::Clock           _lifeClock;

    BackgroundSharedPtr _background;

    Path                _path;
    float               _speed = 2.0f;  // Pixels per timestep

    Direction           _direction = DOWN;  // Current direction of the object
    State               _state = MOVING;

    bool                _finishedPath = false;

};

bool isPathBlocked(const sf::FloatRect& object, const sf::FloatRect& other,
    Direction direction, float minDistance);

template<typename V>
inline sf::Vector2<V> vehicleStepDirection(const sf::Vector2<V>& point, Direction direction, V speed)
{
    switch (direction)
    {
    default:
        break;

    case Direction::UP:
        return { point.x, point.y - speed };

    case Direction::DOWN:
        return { point.x, point.y + speed };

    case Direction::LEFT:
        return { point.x - speed, point.y };

    case Direction::RIGHT:
        return { point.x + speed, point.y };
    }

    return point;
}

} // namespace tt