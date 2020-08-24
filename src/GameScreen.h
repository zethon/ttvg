#pragma once

#include <lua/lua.hpp>

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
    void initLua();

    SceneSharedPtr  _currentScene;
    SceneMap        _scenes;
    PlayerPtr       _player;
    lua_State*      _luaState;
};

} // namespace tt