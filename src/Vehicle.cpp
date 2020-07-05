#include <iostream>
#include <cmath>

#include "TTUtils.h"
#include "Background.h"
#include "Vehicle.h"

namespace tt
{

bool isPathBlocked(const sf::FloatRect& object, const sf::FloatRect& other,
    Vehicle::Direction direction, float minDistance)
{
    //const auto minDistance = 12.5f;
    const auto minPerpDistance = 50.0f;
    // auto globalPosition = sf::Vector2f { getGlobalBounds().left, getGlobalBounds().top };
    // auto currentTile = sf::Vector2i { getTileFromGlobal(globalPosition, _tilesize) };

    const auto[left, top, width, height] = object;
    //auto center = sf::Vector2f { left + (width / 2), top + (height / 2) };


    switch (direction)
    {
        default:
        return false;

        case Vehicle::Direction::UP:
        {
            //float ydiff = currentPos.y - test.y;
            //return std::abs(xdiff) < minPerpDistance
            //    && ydiff >= 0.0f 
            //    && std::abs(ydiff) + (height /2 ) < minDistance;
        }
        break;

        case Vehicle::Direction::DOWN:
        {
            //float ydiff = currentPos.y - test.y;
            //return std::abs(xdiff) < minPerpDistance
            //    && ydiff <= 0.0f 
            //    && std::abs(ydiff) + (height / 2) < minDistance;
        }
        break;

        case Vehicle::Direction::LEFT:
        {
            //float xdiff = currentPos.x - test.x;
            //return std::abs(ydiff) < minPerpDistance
            //    && xdiff >= 0.0f 
            //    && std::abs(xdiff) + (width /2)< minDistance;
        }
        break;

        case Vehicle::Direction::RIGHT:
        {
            const auto xdiff = (left + width) - other.left;

            const auto objectBottom = object.top + object.height;
            const auto otherBottom = other.top + other.height;

            bool yContains = false;
            if (other.height > height)
            {
                yContains = (other.top <= object.top && object.top <= otherBottom)
                    || (other.top <= objectBottom && objectBottom <= otherBottom);
            }
            else
            {
                yContains = (object.top <= other.top && other.top <= objectBottom)
                    || (object.top <= otherBottom && otherBottom <= objectBottom);
            }

            return (std::abs(xdiff) <= minDistance) && yContains;
        }
        break;
    }

    return false;
}

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
        && _movementClock.getElapsedTime().asMilliseconds() > 100)
    {
        move();
        _movementClock.restart();
    }

    if (_lifeClock.getElapsedTime().asSeconds() >= 1000)
    {
        //return TimeStep::DELETE;
    }

    return TimeStep::NOOP;
}

bool Vehicle::isBlocked(const sf::FloatRect& test)
{
    const auto minDistance = 12.5f;
    return isPathBlocked(getGlobalBounds(), test, _direction, minDistance);
}

void Vehicle::move()
{
    const auto speed = 5.9f;

    auto globalPosition = sf::Vector2f { getGlobalBounds().left, getGlobalBounds().top };
    auto currentTile = sf::Vector2i { getTileFromGlobal(globalPosition, _tilesize) };
    auto nextTile = _path.next();

    float xdiff = static_cast<float>(std::pow(currentTile.x - nextTile.x, 2.f));
    float ydiff = static_cast<float>(std::pow(currentTile.y - nextTile.y, 2.f));
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
            currentTile.x -= static_cast<std::int32_t>(speed);
            _direction = LEFT;
        }
        else
        {
            currentTile.x += static_cast<std::int32_t>(speed);
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
            currentTile.y -= static_cast<std::int32_t>(speed);
            _direction = UP;
        }
        else
        {
            currentTile.y += static_cast<std::int32_t>(speed);
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