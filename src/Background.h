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

protected:

    sf::Texture     _texture;

};

} // namespace tt