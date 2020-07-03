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
    if (_movementClock.getElapsedTime().asMilliseconds() > 75)
    {
        move();
    }

    AnimatedSprite::timestep();
    return 0;
}

void Vehicle::move()
{
    auto globalPosition = sf::Vector2f { getGlobalBounds().left, getGlobalBounds().top };
    
    auto currentTile = sf::Vector2u { getTileXY(globalPosition, _tilesize) };
    auto nextTile = _path.next();

    if (currentTile == nextTile)
    {
        _path.step();
        nextTile = _path.next();
    }

    // std::cout << "last: " << _lastPathPoint << '\n';
    // std::cout << "curr: " << currentTile << '\n';
    // std::cout << "next: " << nextTile << '\n';

    auto diff = sf::Vector2i{ nextTile } - sf::Vector2i{ currentTile };
    // std::cout << "diff: " << diff << '\n';

    if (diff.x != 0)
    {
        currentTile.x += diff.x / std::abs(static_cast<float>(diff.x));
        if (diff.x < 0)
        {
            setSource(0, 1);
        }
        else
        {
            setSource(0, 2);
        }
    }
    else if (diff.y != 0)
    {
        currentTile.y += diff.y / std::abs(static_cast<float>(diff.y));
        if (diff.y < 0)
        {
            setSource(0, 3);
        }
        else
        {
            setSource(0, 0);
        }
    }
    

    //auto nextpos = _path.step();
    auto globalPos = getGlobalXY(currentTile, _tilesize);
    setPosition(globalPos);
    _movementClock.restart();
}

} // namespace tt