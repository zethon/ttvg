#pragma once

#include <SFML/Graphics.hpp>

#include "Scenes/Scene.h"

#include "Screen.h"
#include "AnimatedSprite.h"

namespace tt
{
    
class GameScreen : public Screen
{

public:
    GameScreen(ResourceManager& resmgr, sf::RenderTarget& target);

    void draw() override;
    std::uint16_t poll(const sf::Event&) override;
    std::uint16_t timestep() override;

private:
    std::size_t             _currentScene = 1;
    std::vector<ScenePtr>   _scenes;

};

} // namespace tt