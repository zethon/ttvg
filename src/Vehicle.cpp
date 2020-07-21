#include <iostream>
#include <cmath>

#include "TTUtils.h"
#include "Background.h"
#include "Vehicle.h"
#include "Intersection.h"

namespace tt
{

bool isPathBlocked(const sf::FloatRect& object, const sf::FloatRect& other,
    Vehicle::Direction direction, float minDistance)
{
    switch (direction)
    {
        default:
        return false;

        case Vehicle::Direction::UP:
        {
            auto temprect = object;
            temprect.top -= minDistance;
            return temprect.intersects(other);
        }
        break;

        case Vehicle::Direction::DOWN:
        {
            auto temprect = object;
            temprect.top += minDistance;
            return temprect.intersects(other);
        }
        break;

        case Vehicle::Direction::LEFT:
        {
            auto temprect = object;
            temprect.left -= minDistance;
            return temprect.intersects(other);
        }
        break;

        case Vehicle::Direction::RIGHT:
        {
            auto temprect = object;
            temprect.left += minDistance;
            return temprect.intersects(other);
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
    _texture.setSmooth(true);
    setSource(0, 0);
    setScale(1.4f, 1.4f);
    setState(AnimatedSprite::State::ANIMATED);
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

    if (_path.finished())
    {
        return TimeStep::DELETE;
    }

    return TimeStep::NOOP;
}

void Vehicle::move()
{
    auto[xpos, ypos, h, w] = getGlobalBounds();
    auto currentTile = _background->getTileFromGlobal(sf::Vector2f{ xpos, ypos });

    //const auto speed = 1.0f;

    //auto globalPosition = sf::Vector2f{ getGlobalBounds().left, getGlobalBounds().top };
    //auto currentTile = sf::Vector2i{ _background->getTileFromGlobal(globalPosition) };

    //auto currentPathTile = _path.current();
    //auto nextTile = _path.next();

    //float xdiff = static_cast<float>(std::pow(currentTile.x - nextTile.x, 2.f));
    //float ydiff = static_cast<float>(std::pow(currentTile.y - nextTile.y, 2.f));
    //float distance = std::sqrt(xdiff + ydiff);
    //if (distance <= 1.0f)
    //{
    //    if (currentTile == nextTile)
    //    {
    //        _path.step();
    //        nextTile = _path.next();
    //    }
    //    else
    //    {
    //        currentTile = nextTile;
    //    }
    //}

    //auto diff = nextTile - currentTile;

    ////assert(!(diff.x != 0 && diff.y != 0));
    //if (diff.x != 0)
    //{
    //    if (std::abs(diff.x) < speed)
    //    {
    //        currentTile.x += diff.x;
    //    }
    //    else if (diff.x < 0)
    //    {
    //        currentTile.x -= static_cast<std::int32_t>(speed);
    //        _direction = LEFT;
    //    }
    //    else
    //    {
    //        currentTile.x += static_cast<std::int32_t>(speed);
    //        _direction = RIGHT;
    //    }
    //}

    //if (diff.y != 0)
    //{
    //    if (std::abs(diff.y) < speed)
    //    {
    //        currentTile.y += diff.y;
    //    }
    //    else if (diff.y < 0)
    //    {
    //        currentTile.y -= static_cast<std::int32_t>(speed);
    //        _direction = UP;
    //    }
    //    else
    //    {
    //        currentTile.y += static_cast<std::int32_t>(speed);
    //        _direction = DOWN;
    //    }
    //}

    //auto globalPos = _background->getGlobalFromTile(sf::Vector2f{ currentTile });
    //switch (_direction)
    //{
    //default:
    //    break;

    //case UP:
    //    setSource(0, 3);
    //    //globalPos.x -= getGlobalBounds().width / 2;
    //    break;

    //case DOWN:
    //    setSource(0, 0);
    //    //globalPos.x -= getGlobalBounds().width / 2;
    //    break;

    //case LEFT:
    //    setSource(0, 1);
    //    //globalPos.y -= getGlobalBounds().height / 2;
    //    break;

    //case RIGHT:
    //    setSource(0, 2);
    //    //globalPos.y -= getGlobalBounds().height / 2;
    //    break;
    //}


    //// globalPos.x -= getGlobalBounds().width / 2;
    //// globalPos.y -= getGlobalBounds().height / 2;
    //setPosition(globalPos);
}

bool Vehicle::isBlocked(const sf::FloatRect& test)
{
    const auto minDistance = 6.f;
    return isPathBlocked(getGlobalBounds(), test, _direction, minDistance);
}

void Vehicle::setPath(const Path& path)
{
    assert(path.points().size() > 1);

    // VECTOR COPY IN THE GAME LOOP!!!
    _path = path;

    auto& start = path.points().at(0);
    auto& next = path.points().at(1);

    auto direction = tt::getDirection(start, next);
    assert(tt::exactly_one_bit_set(direction));
    _direction = static_cast<Direction>(direction);

    auto globalPos = _background->getGlobalFromTile(sf::Vector2f{ path.points().at(0) });
    setPosition(globalPos);
}

} // namespace tt