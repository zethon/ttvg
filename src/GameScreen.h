#pragma once

#include <lua/lua.hpp>

#include <SFML/Graphics.hpp>

#include "Scenes/Scene.h"

#include "Screen.h"
#include "AnimatedSprite.h"
#include "Player.h"
#include "TTLua.h"

namespace tt
{

constexpr auto GAMESCREEN_LUA_IDX = 3;

int ItemFactory_createItem(lua_State* L)
{
    auto gamescreen = GameScreen::l_get(L);
    
    auto temp = static_cast<Scene**>(luaL_checkudata(L, 1, Scene::CLASS_NAME));
    auto scene = *temp;
    lua_pushstring(L, scene->name().c_str());
    return 1;
}

const struct luaL_Reg ItemFactory_LuaMethods[] =
{
    {"createItem", ItemFactory_createItem},
    {nullptr, nullptr}
};

template<typename T>
void initLua(lua_State* L, T& screen)
{
    luaL_openlibs(L);

    // push a reference to `this` into the registry, it should
    // always be the 3rd entry
    lua_pushlightuserdata(L, static_cast<void*>(&screen));
    luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
    [[maybe_unused]] int reference = luaL_ref(L, LUA_REGISTRYINDEX);
    assert(GAMESCREEN_LUA_IDX == reference);

    //luaL_newmetatable(_luaState, "GameScreen");
    //lua_pushstring(_luaState, "__index");
    //lua_pushvalue(_luaState, -2); // push the metatable
    //lua_settable(_luaState, -3);  // metatable.__index = metatable

    registerLuaFunctions<Scene>(L);
    registerLuaFunctions<Player>(L);
    registerLuaFunctions<DescriptionText>(L);

    
    
    registerStaticLuaType<ItemFactory>(L, "ItemFactory");
    {
        // this creates class-like (~static) methods for a
        // "class" named 'detector, of which we don't need any
        // right now, but I'm putting this here for possible
        // future reference

        lua_newtable(L);
        luaL_setfuncs(L, ItemFactory_LuaMethods, 0);
        lua_setglobal(L, ItemFactory::CLASS_NAME);
    }

    assert(lua_gettop(L) == 0);
}

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
    SceneSharedPtr                  _currentScene;
    SceneMap                        _scenes;
    PlayerPtr                       _player;
    lua_State*                      _luaState;
    std::shared_ptr<ItemFactory>    _itemFactory;
};

} // namespace tt