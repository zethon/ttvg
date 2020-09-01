#pragma once

#include <lua/lua.hpp>

#include <nlohmann/json.hpp>

#include "../Screen.h"
#include "../Player.h"
#include "../Background.h"
#include "../Item.h"
#include "../ItemFactory.h"

#include "Hud.h"
#include "DescriptionText.h"
#include "DebugWindow.h"

namespace tt
{

struct AvatarInfo
{
    sf::Vector2f    start;
    sf::Vector2f    scale;
    sf::Vector2f    source;
    sf::Vector2f    origin;
    float           stepsize;
};

struct CallbackInfo
{
    std::string onInit = "onInit";
    std::string onEnter = "onEnter";
    std::string onExit = "onExit";
};

class Scene;
using ScenePtr = std::unique_ptr<Scene>;
using SceneSharedPtr = std::shared_ptr<Scene>;

void from_json(const nl::json& j, AvatarInfo& av);
void from_json(const nl::json& j, CallbackInfo& cb);

template<typename SceneT>
int loadSceneLuaFile(SceneT& scene, const std::string& filename, lua_State* L)
{
    if (!L) return 0;

    // load the Scene's Lua file into its own sandboxed
    // environment which also contains everything in _G
    {
        lua_newtable(L); // 1:tbl
        if (luaL_loadfile(L, filename.c_str()) != 0) // 1:tbl, 2:chunk
        {
            auto error = lua_tostring(L, -1);
            throw std::runtime_error(error);
        }

        lua_newtable(L); // 1:tbl, 2:chunk, 3:tbl(mt)
        lua_getglobal(L, "_G"); // 1:tbl, 2:chunk, 3:tbl(mt), 4:_G
        lua_setfield(L, 3, "__index"); // 1:tbl, 2:chunk, 3:tbl(mt)
        lua_setmetatable(L, 1); // 1:tbl, 2:chunk
        lua_pushvalue(L, 1); // 1:tbl, 2:chunk, 3:tbl

        lua_setupvalue(L, -2, 1); // 1:tbl, 2:chunk
        if (lua_pcall(L, 0, 0, 0) != 0) // 1:tbl
        {
            auto error = lua_tostring(L, -1);
            throw std::runtime_error(error);
        }

        lua_setglobal(L, scene.name().c_str()); // empty stack
        assert(lua_gettop(L) == 0);
    }

    int idx = 0;

    // create a pointer to `this` in the Lua state and register
    // it as a `Scene` class/object/table inside Lua
    {
        // create the pointer to ourselves in the Lua state
        std::size_t size = sizeof(SceneT*);
        SceneT** data = static_cast<SceneT**>(lua_newuserdata(L, size)); // -1:ud
        *data = &scene;

        // and set the metatable
        luaL_getmetatable(L, SceneT::CLASS_NAME); // -2:ud, -1: mt
        lua_setmetatable(L, -2); // -1: ud
        idx = luaL_ref(L, LUA_REGISTRYINDEX);  // empty stack

        // make sure we're balanced
        assert(lua_gettop(L) == 0);
    }

    return idx;
}

int Scene_getPlayer(lua_State* L);

class Scene : public Screen
{

public:
    static constexpr auto CLASS_NAME = "Scene";
    static const struct luaL_Reg LuaMethods[];

    friend int Scene_getPlayer(lua_State* L);

    Scene(std::string_view name,
        ResourceManager& res,
        sf::RenderTarget& target,
        PlayerPtr player,
        lua_State* luaState);

    std::string name() const { return _name; }

    virtual void init();

    virtual void enter();
    virtual void exit();

    PollResult poll(const sf::Event& e) override;
    void draw() override;

    sf::Vector2f getPlayerTile() const;

    int luaIdx() const { return _luaIdx; }

protected:
    virtual void updateCurrentTile(const TileInfo& info);
    virtual sf::Vector2f animeCallback();
    virtual void adjustView();

    [[maybe_unused]] bool walkPlayer(float speed);    

    std::string             _name;
    lua_State*              _luaState = nullptr;
    int                     _luaIdx = 0;
    CallbackInfo            _callbackNames;

    Hud                     _hud;
    DescriptionText         _descriptionText;
    DebugWindow             _debugWindow;

    BackgroundSharedPtr     _background;

    std::weak_ptr<Player>   _weakPlayer;
    PlayerPtr               _player;
    sf::Vector2f            _lastPlayerPos;
    AvatarInfo              _playerAvatarInfo;
    TileInfo                _currentTile;

    std::vector<ItemPtr>    _items;

private:
    void createItems();
};

} // namespace tt