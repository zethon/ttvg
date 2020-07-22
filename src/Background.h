#pragma once
#include <cmath>
#include <set>
#include <memory>

#include <nlohmann/json.hpp>

#include <SFML/Graphics.hpp>

#include "TTUtils.h"
#include "Tiles.hpp"

namespace nl = nlohmann;

namespace tt
{

inline sf::Vector2f getGlobalFromTile(const sf::Vector2f& tilepos, const sf::Vector2f& tileSize)
{
    auto x = static_cast<float>(tilepos.x * tileSize.x);
    auto y = static_cast<float>(tilepos.y * tileSize.y);
    return { x, y };
}

template<typename Int>
inline sf::Vector2f getGlobalFromTile(Int posx, Int posy, const sf::Vector2f& tileSize)
{
    return getGlobalFromTile(sf::Vector2f{ posx, posy }, tileSize);
}

struct zone_compare
{

bool operator()(const sf::FloatRect& lhs, const sf::FloatRect& rhs) const
{
    return lhs.left < rhs.left;
}

};

using ZoneSet = std::set<sf::FloatRect, zone_compare>;

class ResourceManager;

class Background;
using BackgroundPtr = std::unique_ptr<Background>;
using BackgroundSharedPtr = std::shared_ptr<Background>;

class Background : public sf::Sprite
{

public:
    using Zone = std::tuple<std::string, sf::FloatRect>;

    Background(std::string_view name, ResourceManager& resmgr, const sf::Vector2f& tilesize);

    sf::FloatRect getWorldTileRect() const;

    tt::Tile getTileFromGlobal(const sf::Vector2f& global) const
    {
        return tt::getTileFromGlobal(global, tilesize(), getScale());
    }

    sf::Vector2f getGlobalCenterFromTile(const sf::Vector2f& tile) const
    {
        auto[tilex, tiley] = tilesize();
        auto[scalex, scaley] = getScale();

        auto pos = getGlobalFromTile(tile);
        pos.x += (tilex * scalex) / 2;
        pos.y += (tiley * scaley) / 2;

        return pos;
    }

    sf::Vector2f getGlobalFromTile(const tt::Tile& tile) const
    {
        sf::Vector2f temp;
        temp.x = static_cast<float>(tile.x * getScale().x);
        temp.y = static_cast<float>(tile.y * getScale().y);
        return sf::Vector2f{tt::getGlobalFromTile(temp, _tilesize )};
    }

    sf::Vector2f tilesize() const { return _tilesize; }

    nl::json& json() { return *_json; }
    const nl::json& json() const { return const_cast<const nl::json&>(json()); }

    std::string mapname() const { return _mapname; }

    // TODO: there are some interesting optimizations that
    // could be done here, which might be interesting to do
    // but there are other things I want to do right now
    std::string zoneName(const sf::Vector2f& v);

protected:

    std::unique_ptr<sf::Texture>    _texture;
    std::vector<Zone>               _zones;

private:
    void initZones();

    sf::Vector2f                _tilesize;
    std::unique_ptr<nl::json>   _json;
    std::string                 _mapname;
};

} // namespace tt