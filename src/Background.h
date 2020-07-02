#pragma once
#include <cmath>

#include <SFML/Graphics.hpp>

namespace tt
{

class ResourceManager;

template<typename V>
sf::Vector2f getTileXY(const V& position, const sf::Vector2u& tileSize)
{
    auto x = static_cast<float>(std::floor(position.x / tileSize.x));
    auto y = static_cast<float>(std::floor(position.y / tileSize.y));
    return { x, y };
}

template<typename Int>
sf::Vector2f getTileXY(Int posx, Int posy, const sf::Vector2u& tileSize)
{
    return getTileXY({ posx, posy }, tileSize);
}

class Background;
using BackgroundPtr = std::unique_ptr<Background>;
using BackgroundSharedPtr = std::shared_ptr<Background>;

class Background : public sf::Sprite
{

public:
    using Zone = std::tuple<std::string, sf::FloatRect>;

    Background(std::string_view name, ResourceManager& resmgr);

    sf::FloatRect getWorldRect() const;

    float getLeftBoundary() const { return 0; }
    float getRightBoundary() const;
    float getTopBoundary() const { return 0; }
    float getBottomBoundary() const;

    // TODO: there are some interesting optimizations that
    // could be done here, which might be interesting to do
    // but there are other things I want to do right now
    std::string zoneName(const sf::Vector2f& v);

protected:

    std::unique_ptr<sf::Texture>    _texture;
    std::vector<Zone>               _zones;

};

} // namespace tt