#pragma once
#include <cmath>

#include <boost/any.hpp>

#include <SFML/System/Vector2.hpp>

namespace tt
{

using Tile = sf::Vector2f;
using Size = sf::Vector2f;
using Scale = sf::Vector2f;

enum class TileType
{
    NONE,
    ZONE
};

struct TileInfo
{
    TileType        type;
    boost::any      data;
    Tile            tile;
};

namespace tiles
{

inline Tile getTileFromGlobal(const sf::Vector2f& global, const Size& tilesize, const Scale& scale)
{
    sf::Vector2f temp;
    temp.x = global.x / scale.x;
    temp.y = global.y / scale.y;

    temp.x = std::floor(temp.x / tilesize.x);
    temp.y = std::floor(temp.y / tilesize.y);

    return temp;
}

inline sf::Vector2f getGlobalFromTile(const Tile& tilepos, const Size& tileSize, const Scale& scale)
{
    sf::Vector2f temp;
    
    temp.x = static_cast<float>(tilepos.x * tileSize.x * scale.x);
    temp.y = static_cast<float>(tilepos.y * tileSize.y * scale.y);
    
    return temp;
}

} // tiles
} // namespace tt