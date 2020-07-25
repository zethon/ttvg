#pragma once
#include <functional>

#include <SFML/Graphics.hpp>

#include "IUpdateable.h"

namespace tt
{

class AnimatedSprite;
using AnimatedSpritePtr = std::shared_ptr<AnimatedSprite>;

using AnimeCallback = std::function<void()>;

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

    enum Direction
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    AnimatedSprite(sf::Texture texture, const sf::Vector2i& size);

    void setSource(std::uint32_t x, std::uint32_t y);
    void setState(State state);
    void setMaxFramesPerRow(std::uint32_t max);
    void setAnimeCallback(AnimeCallback cb) { _animeCallback = cb; }

    sf::Texture& texture();
    AnimatedSprite::State state() const;

    std::uint16_t timestep() override;

    Direction direction() const { return _direction; }
    void setDirection(Direction val) { _direction = val; }

    sf::RectangleShape& getHighlightRect() { return _highlight; }

    void    setPosition(float x, float y);
    void    setPosition(const sf::Vector2f& position);

    void    setHighlighted(bool b)  { _isHighlighted = b; }
    bool    isHighlighted()         { return _isHighlighted; }

protected:

    State           _state = State::STILL;
    Direction       _direction = Direction::DOWN;
    sf::Texture     _texture;
    sf::Vector2i    _size;
    sf::Vector2i    _source;
    sf::Clock       _timer;

    // some sprite sheets have different frames per row
    // so this allows us to adjust how many frames get
    // animated in a particular row
    std::uint32_t   _maxFramesPerRow = 0;

    AnimeCallback   _animeCallback;

private:

    bool                _isHighlighted;
    sf::RectangleShape  _highlight;

    virtual void draw(  sf::RenderTarget&   target,
                        sf::RenderStates    states) const;

};

} // namespace tt
