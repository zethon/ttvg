#pragma once
#include <vector>

#include <SFML/Graphics.hpp>

#include "AnimatedSprite.h"

namespace tt
{

class Background;
using BackgroundSharedPtr = std::shared_ptr<Background>;

class Vehicle;
using VehiclePtr = std::shared_ptr<Vehicle>;

class Path
{
    std::vector<sf::Vector2u>   _points;
    float                       _speed;
    std::size_t                 _idx = 0;

public:
    std::vector<sf::Vector2u>& points()
    {
        return _points;
    }

    [[maybe_unused]] sf::Vector2u step()
    {
        if (_idx >= _points.size()) _idx = 0;
        return _points.at(_idx++);
    }

    sf::Vector2u next() const
    {
        auto nextidx = _idx;
        if (nextidx >= _points.size()) nextidx = 0;
        return _points.at(nextidx);
    }
};

class Vehicle : public AnimatedSprite
{
    sf::Clock           _movementClock;
    BackgroundSharedPtr _background;

    Path                _path;
    sf::Vector2u        _lastPathPoint;
    const sf::Vector2u  _tilesize;

public:
    Vehicle(sf::Texture texture, const sf::Vector2i& size, BackgroundSharedPtr bg);

    std::uint16_t timestep() override;

private:
    void move();

};

} // namespace tt