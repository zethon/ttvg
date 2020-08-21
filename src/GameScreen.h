#pragma once

#include <SFML/Graphics.hpp>

#include "Scenes/Scene.h"

#include "Screen.h"
#include "AnimatedSprite.h"
#include "Player.h"

namespace tt
{
    
class GameScreen : public Screen
{

public:
    using SceneMap = std::map<std::string, SceneSharedPtr>;

    GameScreen(ResourceManager& resmgr, sf::RenderTarget& target);

    void draw() override;
    PollResult poll(const sf::Event&) override;
    ScreenAction timestep() override;

private:
    SceneSharedPtr          _currentScene;
    std::vector<ScenePtr>   _scenes;
    SceneMap                _scenes2;
    PlayerPtr               _player;

};

} // namespace tt