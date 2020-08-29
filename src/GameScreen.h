#pragma once

#include <lua/lua.hpp>

#include <SFML/Graphics.hpp>

#include "Scenes/Scene.h"

#include "Screen.h"
#include "AnimatedSprite.h"
#include "Player.h"

namespace tt
{

constexpr auto GAMESCREEN_LUA_IDX = 3;
    
class GameScreen final : public Screen
{

public:
    using SceneMap = std::map<std::string, SceneSharedPtr>;

    static GameScreen* l_get(lua_State* L);

    GameScreen(ResourceManager& resmgr, sf::RenderTarget& target);
    ~GameScreen();

    void draw() override;
    PollResult poll(const sf::Event&) override;
    ScreenAction timestep() override;

    lua_State* lua() const { return _luaState; }
    const SceneMap& scenes() const { return _scenes; }

private:
    void initLua();

    SceneSharedPtr  _currentScene;
    SceneMap        _scenes;
    PlayerPtr       _player;
    lua_State*      _luaState;
};

} // namespace tt