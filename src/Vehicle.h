#pragma once
#include <boost/circular_buffer.hpp>

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
    boost::circular_buffer<sf::Vector2u>    _points;
    float                                   _speed;

public:
    void push_back(const sf::Vector2u& v)
    {
        _points.push_back(v);
    }

    void push_back(std::uint32_t x, std::uint32_t y)
    {
        _points.push_back(sf::Vector2u{x, y});
    }
};

class Vehicle : public AnimatedSprite
{
    Path    _path;
    
    BackgroundSharedPtr _background;

public:
    Vehicle(sf::Texture texture, const sf::Vector2i& size, BackgroundSharedPtr bg);

    std::uint16_t timestep() override;

};

} // namespace tt