#pragma once
#include <functional>

#include <SFML/Graphics.hpp>

#include "GameTypes.h"
#include "IUpdateable.h"

namespace tt
{

class AnimatedSprite;
using AnimatedSpritePtr = std::shared_ptr<AnimatedSprite>;

class AnimatedSprite;
using AnimatedSpritePtr = std::shared_ptr<AnimatedSprite>;

using AnimeCallback = std::function<void()>;

class AnimatedSprite :
    public sf::Drawable,
    public sf::Transformable,
    public IUpdateable
{

public:
    AnimatedSprite(const sf::Texture& texture, const sf::Vector2i& size);

    void setSource(std::uint32_t x, std::uint32_t y);
    void setState(AnimatedState state);
    void setMaxFramesPerRow(std::uint32_t max);
    void setAnimeCallback(AnimeCallback cb) { _animeCallback = cb; }

    AnimatedState state() const;

    std::uint16_t timestep() override;

    Direction direction() const { return _direction; }
    void setDirection(Direction val) { _direction = val; }

    sf::FloatRect getGlobalBounds() const
    {
        const auto textureRect = _sprite.getTextureRect();
        float width = static_cast<float>(std::abs(textureRect.width));
        float height = static_cast<float>(std::abs(textureRect.height));
        return getTransform().transformRect(sf::FloatRect(0.f, 0.f, width, height));
    }

    void setHighlight(bool h);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

    AnimatedState   _state = AnimatedState::STILL;
    Direction       _direction = Direction::DOWN;
    sf::Vector2i    _size;
    sf::Vector2i    _source;
    sf::Clock       _timer;

    // some sprite sheets have different frames per row
    // so this allows us to adjust how many frames get
    // animated in a particular row
    std::uint32_t   _maxFramesPerRow = 0;

    AnimeCallback   _animeCallback;

    sf::Sprite          _sprite;
    sf::RectangleShape  _highlight;
};

} // namespace tt