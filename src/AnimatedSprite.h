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

using AnimeCallback = std::function<sf::Vector2f()>;

class AnimatedSprite :
    public sf::Drawable,
    public sf::Transformable,
    public IUpdateable
{

public:
    AnimatedSprite(const sf::Texture& texture, const sf::Vector2i& size);

    AnimatedState state() const;
    void setState(AnimatedState state);

    Direction direction() const { return _direction; }
    void setDirection(Direction val) { _direction = val; }

    ///
    /// \brief Set the current cell to be drawn
    ///
    void setSource(std::uint32_t x, std::uint32_t y);

    ///
    /// \brief  Set the max width in terms of cells for a
    ///         given row. The animation will cycle through
    ///         this number of cells
    ///
    void setMaxFramesPerRow(std::uint32_t max);
    
    void setHighlighted(bool h);
    bool highlighted() const { return _highlight.getSize().x != 0; }
    
    sf::RectangleShape& highlight() { return _highlight; }

    void setAnimeCallback(AnimeCallback cb) { _animeCallback = cb; }

    sf::FloatRect getGlobalBounds() const;

    std::uint16_t timestep() override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

    const sf::Vector2i      _size; // fixed cell size of each frame within the sprite

    AnimatedState           _state = AnimatedState::STILL;
    Direction               _direction = Direction::DOWN;
    sf::Vector2i            _source;
    sf::Clock               _timer;

    // some sprite sheets have different frames per row
    // so this allows us to adjust how many frames get
    // animated in a particular row
    std::uint32_t           _maxFramesPerRow = 0;

    AnimeCallback           _animeCallback;

    sf::Sprite              _sprite;
    sf::RectangleShape      _highlight;

};

} // namespace tt