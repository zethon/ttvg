#include <iostream>

#include "AnimatedSprite.h"

namespace tt
{

AnimatedSprite::AnimatedSprite(const sf::Texture& texture, const sf::Vector2i& size)
    :   _size{ size }
{
    _isHighlighted = false;

    _sprite.setTexture(texture);
    _sprite.setTextureRect(sf::IntRect(0, 0, _size.x, _size.y));

    _highlight.setFillColor(sf::Color::Transparent);
    _highlight.setOutlineThickness(2);
    _highlight.setOutlineColor(sf::Color(255, 255, 255));
    _highlight.setSize(sf::Vector2f{DEFAULT_HIGHLIGHT_WIDTH,
                                    DEFAULT_HIGHLIGHT_HEIGHT } ); 

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

        setHighlightPosition();

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
    _isHighlighted = h;
    setHighlightPosition();
}

void AnimatedSprite::draw(  sf::RenderTarget& target, 
                            sf::RenderStates states     ) const
{

    states.transform *= getTransform();
    target.draw(_sprite, states);

    if(_isHighlighted)
    {
        target.draw(_highlight);
    }

}

void AnimatedSprite::setHighlightPosition()
{
    auto b = getGlobalBounds();
    
    float x = b.left + ((b.width / 2)   - (DEFAULT_HIGHLIGHT_WIDTH / 2));
    float y = b.top + ((b.height)       - (DEFAULT_HIGHLIGHT_HEIGHT));
    
    _highlight.setPosition( sf::Vector2f{ x, y } );
    
    _highlight.setSize(sf::Vector2f{DEFAULT_HIGHLIGHT_WIDTH,
                                    DEFAULT_HIGHLIGHT_HEIGHT } ); 

    _highlight.setOutlineThickness(2);

}

void AnimatedSprite::setPosition(float x, float y)
{
    Transformable::setPosition(x, y);
	setHighlightPosition();
}

void AnimatedSprite::setPosition(const sf::Vector2f& position)
{
    this->setPosition(position.x, position.y);
}


} // namespace tt
