#pragma once

#include <lua.hpp>

#include <SFML/Graphics.hpp>

#include "../Scenes/Scene.h"
#include "../Scenes/ModalWindow.h"

#include "../Item.h"
#include "../Player.h"
#include "../TTLua.h"
#include "../TTUtils.h"
#include "../TooterLogger.h"
#include "../GameWorld.h"
#include "../Settings.h"
#include "../AudioService.h"

#include "Screen.h"

namespace tt
{

namespace
{

int Utils_openUrl(lua_State* L)
{
    const auto url = lua_tostring(L, 1);
    tt::openBrowser(url);
    return 0;
}

int Utils_showModal(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    const auto text = lua_tostring(L, 2);
    ModalWindow mw{ *scene, };
    mw.setText(text);
    mw.exec();
    return 0;
}

int Utils_showYesNo(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    const auto text = lua_tostring(L, 2);
    OptionsWindow mw{ *scene, };
    mw.setText(text);
    mw.addOption("Yes");
    mw.addOption("No");
    mw.exec();
    if (auto res = mw.selection();
        res.has_value() && *res == 0)
    {
        lua_pushboolean(L, 1);
    }
    else
    {
        lua_pushboolean(L, 0);
    }
    return 1;
}

const struct luaL_Reg Utils_LuaMethods[] =
{
    {"openUrl", Utils_openUrl},
    {"showModal", Utils_showModal},
    {"showYesNo", Utils_showYesNo},
    {nullptr, nullptr}
};

}

int tt_lua_require(lua_State* L);

template<typename T>
void initLua(lua_State* L, T& screen, void* itemFactory, void* resourceManager)
{
    auto logger = log::initializeLogger("Lua");
    logger->info("initializing Lua subsystem");

    luaL_openlibs(L);

    // override `require` with our own version
    lua_pushcfunction(L, tt_lua_require);
    lua_setglobal(L, "require");

    // push a reference to `this` into the registry, it should
    // always be the 3rd entry
    lua_pushlightuserdata(L, static_cast<void*>(&screen));
    luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
    [[maybe_unused]] int reference = luaL_ref(L, LUA_REGISTRYINDEX);
    assert(GAMESCREEN_LUA_IDX == reference);

    lua_pushlightuserdata(L, itemFactory);
    luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
    reference = luaL_ref(L, LUA_REGISTRYINDEX);
    assert(ITEMFACTORY_LUA_IDX == reference);

    lua_pushlightuserdata(L, resourceManager);
    luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
    reference = luaL_ref(L, LUA_REGISTRYINDEX);
    assert(RESOURCEMGR_LUA_IDX == reference);

    // register Audio API
    {
        lua_createtable(L, 0, 0); // global table

        lua_newtable(L);
        luaL_setfuncs(L, AudioLocator::MusicLuaMethods, 0);
        lua_setfield(L, -2, "Music");

        lua_newtable(L);
        luaL_setfuncs(L, AudioLocator::SoundLuaMethods, 0);
        lua_setfield(L, -2, "Sound");

        lua_setglobal(L, "Audio");
    }

    // register static methods for `ItemFactory`
    {
        lua_newtable(L);
        luaL_setfuncs(L, ItemFactory::LuaMethods, 0);
        lua_setglobal(L, ItemFactory::CLASS_NAME);
    }

    // register static methods for `Modal`
    {
        lua_newtable(L);
        luaL_setfuncs(L, Utils_LuaMethods, 0);
        lua_setglobal(L, "Utils");
    }

    // register static methods for `Log`
    {
        lua_newtable(L);
        luaL_setfuncs(L, Logger_LuaMethods, 0);
        lua_setglobal(L, "Log");
    }

    //luaL_newmetatable(_luaState, "GameScreen");
    //lua_pushstring(_luaState, "__index");
    //lua_pushvalue(_luaState, -2); // push the metatable
    //lua_settable(_luaState, -3);  // metatable.__index = metatable

    registerLuaFunctions<Scene>(L);
    registerLuaFunctions<Player>(L);
    registerLuaFunctions<DescriptionText>(L);
    registerLuaFunctions<Item>(L);
    registerLuaFunctions<Zone>(L);
    registerLuaFunctions<ModalWindow>(L);
    registerLuaFunctions<MessagesWindow>(L);
    registerLuaFunctions<OptionsWindow>(L);
    registerLuaFunctions<InventoryWindow>(L);

    {
        lua_newtable(L);
        lua_pushstring(L, "Default");
        lua_pushnumber(L, static_cast<std::uint16_t>(ModalType::Default));
        lua_settable(L, -3);
        lua_pushstring(L, "Messages");
        lua_pushnumber(L, static_cast<std::uint16_t>(ModalType::Messages));
        lua_settable(L, -3);
        lua_pushstring(L, "Options");
        lua_pushnumber(L, static_cast<std::uint16_t>(ModalType::Options));
        lua_settable(L, -3);
        lua_pushstring(L, "Inventory");
        lua_pushnumber(L, static_cast<std::uint16_t>(ModalType::Inventory));
        lua_settable(L, -3);
        lua_setglobal(L, "ModalType");
    }

    {
        lua_newtable(L);
        lua_pushstring(L, "Top");
        lua_pushnumber(L, static_cast<std::uint16_t>(ModalType::Default));
        lua_settable(L, -3);
        lua_pushstring(L, "Center");
        lua_pushnumber(L, static_cast<std::uint16_t>(ModalType::Messages));
        lua_settable(L, -3);
        lua_pushstring(L, "Bottom");
        lua_pushnumber(L, static_cast<std::uint16_t>(ModalType::Options));
        lua_settable(L, -3);
        lua_setglobal(L, "ModalAlignment");
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
    ScreenAction update() override;

    lua_State* lua() const { return _luaState; }
    const SceneMap& scenes() const { return _scenes; }

private:
    void createScenes();

    SceneSharedPtr                  _currentScene;
    SceneMap                        _scenes;

    PlayerPtr                       _player;
    ItemInfo                        _playerObjectInfo;

    lua_State*                      _luaState;
    std::shared_ptr<ItemFactory>    _itemFactory;
    amb::SettingsPtr                _settings;

    sf::Clock                       _gameClock;
    std::shared_ptr<GameWorld>      _gameCalendar;

    std::shared_ptr<Hud>            _hud;
};

} // namespace tt
