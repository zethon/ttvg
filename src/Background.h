#pragma once

#include <SFML/Graphics.hpp>

namespace tt
{

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