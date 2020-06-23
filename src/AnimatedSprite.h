#pragma once

#include <SFML/Graphics.hpp>

#include "IUpdateable.h"

namespace tt
{

class AnimatedSprite;
using AnimatedSpritePtr = std::shared_ptr<AnimatedSprite>;

class AnimatedSprite : 
    public sf::Sprite,
    public IUpdateable
{

public:
    enum State
    {
        STILL,
        ANIMATED
    };

    AnimatedSprite(sf::Texture texture, const sf::Vector2i& size);

    void setSource(std::uint32_t x, std::uint32_t y);
    void setState(State state);
    void setMaxFramesPerRow(std::uint32_t max);

    sf::Texture& texture();
    AnimatedSprite::State state() const;

    std::uint16_t timestep() override;

protected:

    State           _state = State::STILL;
    sf::Texture     _texture;
    sf::Vector2i    _size;
    sf::Vector2i    _source;
    sf::Clock       _timer;

    // some sprite sheets have different frames per row
    // so this allows us to adjust how many frames get
    // animated in a particular row
    std::uint32_t   _maxFramesPerRow = 0;
};

class MainCharacter : public AnimatedSprite
{
public:
    using AnimatedSprite::AnimatedSprite;
};

} // namespace tt