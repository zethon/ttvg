#pragma once
#include <cmath>

#include <SFML/System/Vector2.hpp>

namespace tt
{
    
using Tile = sf::Vector2f;
using Size = sf::Vector2f;
using Scale = sf::Vector2f;

inline Tile getTileFromGlobal(const sf::Vector2f& global, const Size& tilesize, const Scale& scale)
{
    sf::Vector2f temp;
    temp.x = global.x / scale.x;
    temp.y = global.y / scale.y;

    temp.x = std::floor(temp.x / tilesize.x);
    temp.y = std::floor(temp.y / tilesize.y);

    return temp;
}

//template<typename V>
//Tile getTileFromGlobal(const V& global, const Size& tilesize, const Scale& scale)
//{
//    return getTileFromGlobal(sf::Vector2f { global }, tilesize, scale);
//}

} // namespace tt