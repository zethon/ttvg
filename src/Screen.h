#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"

namespace tt
{

class Screen
{
    using DrawablePtr = std::shared_ptr<sf::Drawable>;
    std::vector<DrawablePtr>   _objects;

public:
    explicit Screen(ResourceManager& res)
        : _resources{ res }
    {}

    void addDrawable(std::shared_ptr<sf::Text> drawable)
    {
        _objects.push_back(drawable);
    }

    virtual void draw(sf::RenderWindow& window)
    {
        for (const auto& object : _objects)
        {
            window.draw(*object);
        }
    }

    // poll system events
    virtual void poll()
    {}

protected:
    ResourceManager&            _resources;
};

class IntroScreen : public Screen
{
    sf::Font    _font;

public:
    explicit IntroScreen(ResourceManager& mgr)
        : Screen(mgr)
    {
        auto temp =_resources.loadFont("hobo.ttf");
        if (!temp.has_value())
        {
            throw std::runtime_error("hobo.ttf could not be loaded!");
        }

        _font = *temp; // copy?

        auto text = std::make_shared<sf::Text>("Hello Hobo!", _font);
        text->setPosition(120, 120);
        addDrawable(text);
    }
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