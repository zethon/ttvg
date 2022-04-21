#pragma once
#include <cmath>
#include <set>
#include <memory>
#include <optional>

#include <nlohmann/json.hpp>

#include <SFML/Graphics.hpp>

#include "TTUtils.h"
#include "Tiles.hpp"
#include "Transition.h"
#include "Zone.h"

namespace nl = nlohmann;

namespace tt
{

class ResourceManager;

class Background;
using BackgroundPtr = std::unique_ptr<Background>;
using BackgroundSharedPtr = std::shared_ptr<Background>;

class Background : public sf::Sprite
{
    struct zone_compare
    {
        bool operator()(const Zone& z1, const Zone& z2) const
        {
            auto lhs = z1.rect;
            auto rhs = z2.rect;

            if (lhs.left == rhs.left)
            {
                return lhs.top < rhs.top;
            }

            return lhs.left < rhs.left;
        }
    };

    using ZoneSet = std::set<Zone, zone_compare>;

public:
    enum class CameraType { FIXED, FOLLOW };

    Background(std::string_view name, ResourceManager& resmgr, sf::RenderTarget& target);
    Background(std::string_view name, ResourceManager& resmgr, sf::RenderTarget& target, const sf::Vector2f& tilesize);

    sf::FloatRect getWorldTileRect() const;

    tt::Tile getTileFromGlobal(const sf::Vector2f& global) const
    {
        return tiles::getTileFromGlobal(global, tilesize(), getScale());
    }

    tt::Tile getTileFromGlobal(float x, float y)
    {
        return this->getTileFromGlobal(sf::Vector2f{x,y});
    }

    sf::Vector2f getGlobalFromTile(const tt::Tile& tile) const
    {
        return tiles::getGlobalFromTile(tile, tilesize(), getScale());
    }

    tt::Tile getGlobalFromTile(float x, float y)
    {
        return this->getGlobalFromTile(sf::Vector2f{x,y});
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

    sf::Vector2f tilesize() const { return _tilesize; }

    nl::json& json() { return *_json; }
    const nl::json& json() const { return const_cast<const nl::json&>(json()); }

    std::string mapname() const { return _mapname; }

    TileInfo getTileInfo(const sf::Vector2f& v);

    CameraType cameraType() const { return _cameraType; }

    float minzoom() const { return _minzoom; }
    float maxzoom() const { return _maxzoom; }

protected:
    std::unique_ptr<sf::Texture>    _texture;
    ZoneSet                         _zones;

private:
    void initBackground(const sf::RenderTarget& target);
    void initZones();

    sf::Vector2f                _tilesize;
    std::unique_ptr<nl::json>   _json;
    std::string                 _mapname;
    CameraType                  _cameraType = CameraType::FIXED;

    float               _minzoom = 0.f;
    float               _maxzoom = 0.f;

};

} // namespace tt