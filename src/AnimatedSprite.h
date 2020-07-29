#pragma once
#include <functional>

#include <SFML/Graphics.hpp>

#include "GameTypes.h"
#include "IUpdateable.h"

namespace tt
{

    //
    // Default highlight size.
    //
    constexpr auto DEFAULT_HIGHLIGHT_WIDTH  = 32.0f;
    constexpr auto DEFAULT_HIGHLIGHT_HEIGHT = 32.0f;


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
    
    void setHighlight(bool h);
    bool highlighted() const { return _isHighlighted; }
    sf::RectangleShape& highlight() { return _highlight; }

    void setAnimeCallback(AnimeCallback cb) { _animeCallback = cb; }

    sf::FloatRect getGlobalBounds() const;

    std::uint16_t timestep() override;

    void setPosition(float x, float y);
    void setPosition(const sf::Vector2f& position);

protected:

    void draw(  sf::RenderTarget& target, 
                sf::RenderStates states ) const override final;

    void setHighlightPosition();

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

    bool                    _isHighlighted;

};

} // namespace tt
