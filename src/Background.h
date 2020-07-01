#pragma once

#include <SFML/Graphics.hpp>

namespace tt
{

template<typename V>
sf::Vector2u getTileXY(const V& position, const sf::Vector2u& tileSize)
{
    auto x = static_cast<std::uint32_t>(std::floor(position.x / tileSize.x));
    auto y = static_cast<std::uint32_t>(std::floor(position.y / tileSize.y));
    return { x, y };
}

template<typename Int>
sf::Vector2u getTileXY(Int posx, Int posy, const sf::Vector2u& tileSize)
{
    return getTileXY({ posx, posy }, tileSize);
}

class Background;
using BackgroundPtr = std::unique_ptr<Background>;
using BackgroundSharedPtr = std::shared_ptr<Background>;

class Background : public sf::Sprite
{

public:
    explicit Background(sf::Texture texture);

    sf::Texture& texture() { return _texture; }

    sf::FloatRect getWorldRect() const;

    float getLeftBoundary() const { return 0; }
    float getRightBoundary() const;
    float getTopBoundary() const { return 0; }
    float getBottomBoundary() const;

protected:

    sf::Texture     _texture;

};

} // namespace tt