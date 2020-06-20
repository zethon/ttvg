#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"

namespace tt
{

constexpr std::uint16_t SCREEN_INTRO = 0;
constexpr std::uint16_t SCREEN_SHART = 1;
constexpr std::uint16_t SCREEN_GAME = 2;
constexpr std::uint16_t SCREEN_OUTRO = 3;

class Screen
{
    using DrawablePtr = std::shared_ptr<sf::Drawable>;
    std::vector<DrawablePtr>   _objects;

public:
    explicit Screen(ResourceManager& res, sf::RenderTarget& target)
        : _resources{ res },
          _window{target}
    {}

    void addDrawable(std::shared_ptr<sf::Drawable> drawable)
    {
        _objects.push_back(drawable);
    }

    virtual void draw()
    {
        for (const auto& object : _objects)
        {
            _window.draw(*object);
        }
    }

    // poll system/user events
    virtual std::uint16_t poll(const sf::Event&)
    {
        return 0;
    }

    // update positions and state
    virtual std::uint16_t timestep()
    {
        return 0;
    }

protected:
    ResourceManager&    _resources;
    sf::RenderTarget&   _window;
};

class ExitScreen : public Screen
{
public:
    using Screen::Screen;
};

} // namespace tt