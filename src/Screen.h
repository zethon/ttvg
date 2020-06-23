#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"
#include "IUpdateable.h"

namespace tt
{

constexpr std::uint16_t SCREEN_INTRO = 0;
constexpr std::uint16_t SCREEN_SHART = 1;
constexpr std::uint16_t SCREEN_GAME = 2;
constexpr std::uint16_t SCREEN_OUTRO = 3;

using DrawablePtr = std::shared_ptr<sf::Drawable>;

class Screen
{

public:
    Screen(ResourceManager& res, sf::RenderTarget& target);

    virtual ~Screen() = default;

    void addDrawable(DrawablePtr drawable);
    void clearDrawable();

    void addUpdateable(IUpdateablePtr updateable);
    void clearUpdateable();

    // iterate all draw'able obects
    virtual void draw();

    // poll system/user events
    virtual std::uint16_t poll(const sf::Event&);

    // update positions and state
    virtual std::uint16_t timestep();

    // clean up any resources
    virtual void close()
    {
        clearDrawable();
        clearUpdateable();
    }

protected:
    std::vector<DrawablePtr>        _objects;
    std::vector<IUpdateablePtr>     _updateables;

    ResourceManager&                _resources;
    sf::RenderTarget&               _window;

};

} // namespace tt