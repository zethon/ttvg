#pragma once

#include <SFML/Graphics.hpp>

#include "../Background.h"
#include "../AnimatedSprite.h"

#include "Scene.h"

namespace tt
{

class Opening : public Scene
{

public:
    Opening(ResourceManager& resmgr, sf::RenderTarget& target);

    std::uint16_t poll(const sf::Event& e) override; 

private:

    BackgroundSharedPtr     _background;
    AnimatedSpritePtr       _player;

};

} // namespace tt