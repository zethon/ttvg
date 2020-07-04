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
    _direction = RIGHT;

    _lastPathPoint = _path.step();
    auto globalPos = getGlobalFromTile(_lastPathPoint, _tilesize);
    setPosition(globalPos);
}

std::uint16_t Vehicle::timestep()
{
    AnimatedSprite::timestep();
    
    if (_state == State::MOVING
        && _movementClock.getElapsedTime().asMilliseconds() > 65)
    {
        move();
        _movementClock.restart();
    }

    if (_lifeClock.getElapsedTime().asSeconds() >= 100)
    {
        return TimeStep::DELETE;
    }

    return TimeStep::NOOP;
}

bool Vehicle::isBlocked(const sf::Vector2f& test)
{
    const auto minDistance = 100.0f;
    const auto minPerpDistance = 100.0f;
    // auto globalPosition = sf::Vector2f { getGlobalBounds().left, getGlobalBounds().top };
    // auto currentTile = sf::Vector2i { getTileFromGlobal(globalPosition, _tilesize) };
    auto currentPos = sf::Vector2f { getGlobalBounds().left, getGlobalBounds().top };

    float xdiff = currentPos.x - test.x;
    float ydiff = currentPos.y - test.y;

    switch (_direction)
    {
        default:
        return false;

        case UP:
        {
            float ydiff = currentPos.y - test.y;
            return std::abs(xdiff) < minPerpDistance
                && ydiff >= 0.0f 
                && std::abs(ydiff) < minDistance;
        }
        break;

        case DOWN:
        {
            float ydiff = currentPos.y - test.y;
            return std::abs(xdiff) < minPerpDistance
                && ydiff <= 0.0f 
                && std::abs(ydiff) < minDistance;
        }
        break;

        case LEFT:
        {
            float xdiff = currentPos.x - test.x;
            return std::abs(ydiff) < minPerpDistance
                && xdiff >= 0.0f 
                && std::abs(xdiff) < minDistance;
        }
        break;

        case RIGHT:
        {
            float xdiff = currentPos.x - test.x;
            return std::abs(ydiff) < minPerpDistance
                && xdiff <= 0.0f 
                && std::abs(xdiff) < minDistance;
        }
        break;
    }
    return false;
}

void Vehicle::move()
{
    const auto speed = 5.9f;

    auto globalPosition = sf::Vector2f { getGlobalBounds().left, getGlobalBounds().top };
    auto currentTile = sf::Vector2i { getTileFromGlobal(globalPosition, _tilesize) };
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
            _direction = LEFT;
        }
        else
        {
            currentTile.x += speed;
            _direction = RIGHT;
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
            _direction = UP;
        }
        else
        {
            currentTile.y += speed;
            _direction = DOWN;
        }
    }

    switch (_direction)
    {
        default:
        break;

        case UP:
            setSource(0, 3);
        break;

        case DOWN:
            setSource(0, 0);
        break;

        case LEFT:
            setSource(0, 1);
        break;
        
        case RIGHT:
            setSource(0, 2);
        break;
    }
    
    auto globalPos = getGlobalFromTile(currentTile, _tilesize);
    setPosition(globalPos);
}

} // namespace tt