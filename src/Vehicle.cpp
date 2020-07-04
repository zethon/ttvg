#include <iostream>
#include <cmath>

#include "TTUtils.h"
#include "Background.h"
#include "Vehicle.h"

namespace tt
{

Vehicle::Vehicle(sf::Texture texture, const sf::Vector2i& size, BackgroundSharedPtr bg)
    : AnimatedSprite(texture, size),
      _background { bg },
      _tilesize { _background->tilesize() }
{
    _path.points().emplace_back(47,266);
    _path.points().emplace_back(153,266);
    _path.points().emplace_back(153,338);
    _path.points().emplace_back(47,338);

    _texture.setSmooth(true);
    setSource(0, 1);
    setScale(1.80f, 1.80f);
    setState(AnimatedSprite::State::ANIMATED);

    _lastPathPoint = _path.step();
    auto globalPos = getGlobalXY(_lastPathPoint, _tilesize);
    setPosition(globalPos);
}

std::uint16_t Vehicle::timestep()
{
    if (_movementClock.getElapsedTime().asMilliseconds() > 65)
    {
        move();
        _movementClock.restart();
    }

    AnimatedSprite::timestep();

    if (_lifeClock.getElapsedTime().asSeconds() >= 10)
    {
        return TimeStep::DELETE;

    }
    return TimeStep::NOOP;
}

void Vehicle::move()
{
    const auto speed = 5.9f;

    auto globalPosition = sf::Vector2f { getGlobalBounds().left, getGlobalBounds().top };
    auto currentTile = sf::Vector2i { getTileXY(globalPosition, _tilesize) };
    auto nextTile = _path.next();

    float xdiff = std::pow(currentTile.x - nextTile.x, 2.f);
    float ydiff = std::pow(currentTile.y - nextTile.y, 2.f);
    float distance = std::sqrt(xdiff + ydiff);
    if (distance < 3.0f)
    {
        _path.step();
        nextTile = _path.next();
    }

    auto diff = nextTile - currentTile;
    if (diff.x != 0)
    {
        if (std::abs(diff.x) < speed)
        {
            currentTile.x += diff.x;
        }
        else if (diff.x < 0)
        {
            currentTile.x -= speed;
            setSource(0, 1);
        }
        else
        {
            currentTile.x += speed;
            setSource(0, 2);
        }
    }
    
    if (diff.y != 0)
    {
        if (std::abs(diff.y) < speed)
        {
            currentTile.y += diff.y;
        }
        else if (diff.y < 0)
        {
            currentTile.y -= speed;
            setSource(0, 3);
        }
        else
        {
            currentTile.y += speed;
            setSource(0, 0);
        }
    }
    
    auto globalPos = getGlobalXY(currentTile, _tilesize);
    setPosition(globalPos);
}

} // namespace tt