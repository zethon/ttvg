#include <iostream>

#include "AnimatedSprite.h"

namespace tt
{

AnimatedSprite::AnimatedSprite(const sf::Texture& texture, const sf::Vector2i& size)
    :   _size{ size }
{
    _sprite.setTexture(texture);
    _sprite.setTextureRect(sf::IntRect(0, 0, _size.x, _size.y));

    _highlight.setFillColor(sf::Color::Transparent);
    _highlight.setOutlineThickness(2);
    _highlight.setOutlineColor(sf::Color(255, 255, 255));
}

void AnimatedSprite::setSource(std::uint32_t x, std::uint32_t y)
{
    _source.x = x;
    _source.y = y;
    _sprite.setTextureRect(sf::IntRect(
        _source.x * _size.x, _source.y * _size.y, _size.x, _size.y));
}

void AnimatedSprite::setState(AnimatedState state)
{
    _state = state;
    _timer.restart();
}

void AnimatedSprite::setMaxFramesPerRow(std::uint32_t max)
{
    _maxFramesPerRow = max;
}

AnimatedState AnimatedSprite::state() const
{
    return _state;
}

std::uint16_t AnimatedSprite::timestep()
{
    if (AnimatedState::ANIMATED == _state
        && _timer.getElapsedTime().asMilliseconds() > 65)
    {
        auto[left, top] = _source;
        left++;

        auto textureWidth = _maxFramesPerRow > 0 ?
            _maxFramesPerRow * _size.x : _sprite.getTexture()->getSize().x;

        if (textureWidth <= static_cast<std::uint32_t>(left * _size.x))
        {
            left = 0;
        }

        setSource(left, top);

        if (_animeCallback)
        {
            _animeCallback();
        }

        _highlight.setPosition(getPosition());

        _timer.restart();
    }

    return 0;
}

sf::FloatRect AnimatedSprite::getGlobalBounds() const
{
    const auto textureRect = _sprite.getTextureRect();
    float width = static_cast<float>(std::abs(textureRect.width));
    float height = static_cast<float>(std::abs(textureRect.height));
    return getTransform().transformRect(sf::FloatRect(0.f, 0.f, width, height));
}

void AnimatedSprite::setHighlight(bool h)
{
    if (h)
    {
        _highlight.setPosition(getPosition());
        auto b = getGlobalBounds();
        _highlight.setSize(sf::Vector2f{ b.width, b.height });
    }
    else
    {
        _highlight.setSize(sf::Vector2f{ 0.f, 0.f });
    }

}

void AnimatedSprite::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(_sprite, states);
    target.draw(_highlight);
}

} // namespace tt