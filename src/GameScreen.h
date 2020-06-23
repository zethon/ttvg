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

    sf::Texture _bgtext;
    std::shared_ptr<sf::Sprite> _bgsprite;

    sf::View    _view;

public:
    GameScreen(ResourceManager& resmgr, sf::RenderTarget& target);

    std::uint16_t poll(const sf::Event&) override;
    std::uint16_t timestep() override;

};

} // namespace tt