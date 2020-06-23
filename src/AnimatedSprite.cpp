#include <iostream>

#include "AnimatedSprite.h"

namespace tt
{

AnimatedSprite::AnimatedSprite(sf::Texture texture, const sf::Vector2i& size)
    : _texture{ std::move(texture) },
      _size{ size }
{
    setTexture(_texture);
    setTextureRect(sf::IntRect(0, 0, _size.x, _size.y));
}

void AnimatedSprite::setSource(std::uint32_t x, std::uint32_t y)
{
    _source.x = x;
    _source.y = y;
    setTextureRect(sf::IntRect(
        _source.x * _size.x, _source.y * _size.y, _size.x, _size.y));
}

void AnimatedSprite::setState(State state) 
{ 
    _state = state;
    _timer.restart();
}

void AnimatedSprite::setMaxFramesPerRow(std::uint32_t max) 
{ 
    _maxFramesPerRow = max; 
}

sf::Texture& AnimatedSprite::texture()
{
    return _texture;
}

AnimatedSprite::State AnimatedSprite::state() const
{
    return _state;
}

std::uint16_t AnimatedSprite::timestep()
{
    if (State::ANIMATED == _state
        && _timer.getElapsedTime().asMilliseconds() > 100)
    {
        auto [left, top] = _source;
        left++;

        auto textureWidth = _maxFramesPerRow > 0 ?
            _maxFramesPerRow * _size.x : _texture.getSize().x;

        if (textureWidth <= static_cast<std::uint32_t>(left * _size.x))
        {
            left = 0;
        }

        setSource(left, top);
        _timer.restart();
    }

    return 0;
}

} // namespace tt