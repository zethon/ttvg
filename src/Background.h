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

    Background(std::string_view name, ResourceManager& resmgr, const sf::Vector2i& tilesize);

    sf::FloatRect getWorldTileRect() const;

    tt::Tile getTileFromGlobal(const sf::Vector2f& global) const
    {
        return tt::getTileFromGlobal(global, tilesize(), getScale());
    }

    template<typename T>
    sf::Vector2<T> getGlobalFromTile(const sf::Vector2<T>& tile) const
    {
        sf::Vector2<T> temp;
        temp.x = static_cast<T>(tile.x * getScale().x);
        temp.y = static_cast<T>(tile.y * getScale().y);

        // TODO: GET RID OF THIS CAST
        return sf::Vector2<T>{tt::getGlobalFromTile(temp, sf::Vector2i{ _tilesize })};
    }

    void setTileSize(const sf::Vector2i& val) 
    { 
        // TODO: GET RID OF THIS
        _tilesize = sf::Vector2f{ val }; 
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