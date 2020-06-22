#pragma once

#include <SFML/Graphics.hpp>

#include "Screen.h"
#include "AnimatedSprite.h"

namespace tt
{
    
class GameScreen : public Screen
{
    sf::Font    _font;

    std::vector<AnimatedSpritePtr> _animes;
    
    AnimatedSpritePtr   _tt;

public:
    GameScreen(ResourceManager& resmgr, sf::RenderTarget& target);

    std::uint16_t poll(const sf::Event&) override;
    std::uint16_t timestep() override;

};

} // namespace tt