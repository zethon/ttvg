#pragma once

#include <SFML/Graphics.hpp>

#include "Screen.h"

namespace tt
{
    
class GameScreen : public Screen
{
    sf::Font    _font;

public:
    GameScreen(ResourceManager& resmgr, sf::RenderTarget& target)
        : Screen(resmgr, target)
    {
        if (auto temp = _resources.load<sf::Font>("hobo.ttf");
            temp.has_value())
        {
            _font = *temp;
        }
        else
        {
            throw std::runtime_error("hobo.ttf could not be loaded!");
        }

        auto textobj = std::make_shared<sf::Text>("PLAY THE GAME", _font);
        textobj->setPosition(100, 100);
        textobj->setFillColor(sf::Color(255, 1, 129));

        addDrawable(textobj);
    }
};

} // namespace tt