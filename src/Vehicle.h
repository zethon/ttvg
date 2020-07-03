#pragma once

#include <SFML/Graphics.hpp>

#include "AnimatedSprite.h"

namespace tt
{

class Vehicle;
using VehiclePtr = std::shared_ptr<Vehicle>;

class Path
{

public:

};

class Vehicle : public AnimatedSprite
{
public:
    Vehicle(sf::Texture texture, const sf::Vector2i& size);

};

} // namespace tt