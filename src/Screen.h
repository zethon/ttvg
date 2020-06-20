#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"

namespace tt
{

///////////////////////////////
// Screen
///////////////////////////////

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
    virtual void poll()
    {}

    // update positions and state
    virtual void timestep()
    {}

protected:
    ResourceManager&    _resources;
    sf::RenderTarget&   _window;
};

class GameScreen : public Screen
{
public:
    using Screen::Screen;
};

class ExitScreen : public Screen
{
public:
    using Screen::Screen;
};

} // namespace tt