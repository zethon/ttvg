#include <iostream>

#include "AnimatedSprite.h"

namespace tt
{

constexpr auto HIGHLIGHT_WIDTH  = 32;
constexpr auto HIGHLIGHT_HEIGHT = 32;

AnimatedSprite::AnimatedSprite( const sf::Texture&  texture, 
                                const sf::Vector2i& size)
    :   _size{ size }
{
    setTexture(texture);
    setTextureRect(sf::IntRect(0, 0, _size.x, _size.y));

    //
    // Initialize the highlight box.
    //
    _isHighlighted = false;
    _highlight.setFillColor(sf::Color::Transparent);
    _highlight.setOutlineThickness(2);
    _highlight.setOutlineColor(sf::Color(255, 255, 255));
    _highlight.setSize(sf::Vector2f(HIGHLIGHT_WIDTH, HIGHLIGHT_WIDTH));
}

void AnimatedSprite::setSource(std::uint32_t x, std::uint32_t y)
{
    _source.x = x;
    _source.y = y;
    setTextureRect(sf::IntRect(
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
            _maxFramesPerRow * _size.x : getTexture()->getSize().x;

        if (textureWidth <= static_cast<std::uint32_t>(left * _size.x))
        {
            left = 0;
        }

        setSource(left, top);

        if (_animeCallback)
        {
            _animeCallback();
        }

        _timer.restart();
    }

    return 0;
}

void AnimatedSprite::setPosition(float x, float y)
{
    sf::Sprite::setPosition(x, y);

    //
    // Compute position of hightlight box.
    //
    auto hx = getGlobalBounds().left +
                ((getGlobalBounds().width / 2) - (HIGHLIGHT_WIDTH / 2));

    auto hy = getGlobalBounds().top + 
                ((getGlobalBounds().height) - (HIGHLIGHT_HEIGHT));

    _highlight.setPosition(hx, hy);
}

void AnimatedSprite::setPosition(const sf::Vector2f& position)
{
    this->setPosition(position.x, position.y);
}

void AnimatedSprite::draw(  sf::RenderTarget&   target,
                            sf::RenderStates    states) const
{
    target.draw(static_cast<Sprite>(*this));

    if(_isHighlighted)
    {
        target.draw(_highlight, states);
    }
}

} // namespace tt
