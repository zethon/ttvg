#pragma once

#include <lua.hpp>

#include <nlohmann/json.hpp>

#include "../Screens/Screen.h"

#include "../Player.h"
#include "../Background.h"
#include "../ItemFactory.h"
#include "../TooterLogger.h"
#include "../DelayedSound.h"
#include "../GameWorld.h"

#include "Hud.h"
#include "DescriptionText.h"
#include "DebugWindow.h"
#include "ModalWindow.h"

namespace tt
{

constexpr auto DEFAULT_STEPSIZE = 16.0f;

struct AvatarInfo
{
    sf::Vector2f    start;
    sf::Vector2f    scale;
    sf::Vector2f    source;
    sf::Vector2f    origin;
    float           stepsize = DEFAULT_STEPSIZE;
    std::string     state;
};

struct CallbackInfo
{
    std::string onInit = "onInit";
    std::string onEnter = "onEnter";
    std::string onExit = "onExit";
    std::string onTileUpdate = "onTileUpdate";
};

class Scene;
using ScenePtr = std::unique_ptr<Scene>;
using SceneSharedPtr = std::shared_ptr<Scene>;

struct SceneSetup
{
    ResourceManager&                resources;
    sf::RenderTarget&               window;
    PlayerPtr                       player;
    lua_State*                      lua;
    std::shared_ptr<ItemFactory>    itemFactory;
    std::shared_ptr<GameWorld>      calendar;
    std::shared_ptr<Hud>            hud;
};

void from_json(const nl::json& j, AvatarInfo& av);
void from_json(const nl::json& j, CallbackInfo& cb);

template<typename SceneT>
bool loadSceneLuaFile(SceneT& scene, const std::string& filename, lua_State* L)
{
    if (!L) return false;

    auto logger = log::initializeLogger("Lua");
    logger->debug("loading scene lua file {}", filename);

    // load the Scene's Lua file into its own sandboxed
    // environment which also contains everything in _G
    {
        lua_newtable(L); // 1:tbl
        if (luaL_loadfile(L, filename.c_str()) != 0) // 1:tbl, 2:chunk
        {
            auto error = lua_tostring(L, -1);
            logger->error("could not load scene lua file '{}' because: {}", filename, error);
            lua_settop(L, 0);
            return false;
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
            logger->error("could not load scene lua file '{}' because: {}", filename, error);
            lua_settop(L, 0);
            return false;
        }

        lua_setglobal(L, scene.name().c_str()); // empty stack
        assert(lua_gettop(L) == 0);
    }

    return true;
}

template<typename ObjT>
int registerObject(lua_State* L, ObjT& object)
{
    int idx = 0;

    // create a pointer to the object in the Lua state and register
    // it as an object of `ObjT` class/object/table inside Lua
    {
        // create the pointer to itself in the Lua state
        std::size_t size = sizeof(ObjT*);
        ObjT** data = static_cast<ObjT**>(lua_newuserdata(L, size)); // -1:ud
        *data = &object;

        // and set the metatable
        luaL_getmetatable(L, ObjT::CLASS_NAME); // -2:ud, -1: mt
        lua_setmetatable(L, -2); // -1: ud
        idx = luaL_ref(L, LUA_REGISTRYINDEX);  // empty stack

        // make sure we're balanced
        assert(lua_gettop(L) == 0);
    }

    return idx;
}

int Scene_getPlayer(lua_State* L);
int Scene_getDescriptionWindow(lua_State* L);

struct BackgroundMusic
{
    std::string file;

    float volume    = 100.0f;
    bool loop       = false;
    bool enabled    = false;
};

void from_json(const nl::json& j, BackgroundMusic& bm);

class Scene : public Screen
{

public:
    using Items = std::vector<ItemPtr>;
    using ItemTasks = std::map<sf::Time, ItemInstanceInfo>;

    static constexpr auto CLASS_NAME = "Scene";
    static const struct luaL_Reg LuaMethods[];

    friend int Scene_getPlayer(lua_State* L);
    friend int Scene_getDescriptionWindow(lua_State* L);
    
    Scene(std::string_view name, const SceneSetup& setup);

    std::string name() const { return _sceneName; }

    virtual void init();

    virtual void enter();
    virtual void exit();

    PollResult poll(const sf::Event& e) override;
    void draw() override;
    virtual ScreenAction update(sf::Time elapsed);

    sf::Vector2f getPlayerTile() const;
    void setPlayerTile(const Tile& tile);

    int luaIdx() const { return _luaIdx; }

//    Hud& hud() { return _hud; }
    DescriptionText& descriptionText() { return _descriptionText; }

    void addItem(ItemPtr item);
    void removeItem(ItemPtr item);
    const std::vector<ItemPtr>& items() const { return _items; }
    void placeItem(ItemPtr item);
    void pickupItem(Items::iterator itemIt);

    BackgroundSharedPtr background() const { return _background; }
    PlayerPtr player() const { return _player; }

protected:
    virtual sf::Vector2f animeCallback();
    virtual void adjustView();
    
    // subclasses might also have to deal with highlighting
    virtual void toggleHighlight();

    [[maybe_unused]] bool walkPlayer(float baseStepSize);
    void showHelp();

    std::string     _sceneName;
    lua_State*      _luaState = nullptr;
    int             _luaIdx = 0;    // the index for `this` object in the Lua registry
    CallbackInfo    _callbackNames;

    DescriptionText _descriptionText;
    DebugWindow     _debugWindow;

    BackgroundSharedPtr         _background;
    std::string     _bgSongName;

    std::weak_ptr<Player>   _weakPlayer;
    PlayerPtr               _player;

    std::weak_ptr<Hud>      _weakHud;
    std::shared_ptr<Hud>    _hudPtr;

    sf::Vector2f            _lastPlayerPos;
    AvatarInfo              _playerAvatarInfo;
    TileInfo                _currentTile;

    Items               _items;
    ItemTasks           _itemTasks;
    ItemFactory&        _itemFactory;
    ItemInfoMap         _objectInfoList;

    log::SpdLogPtr  _logger;

    sf::Time        _gameTime;

    DelayedSoundPtr _walkSound;
    DelayedSoundPtr _pickupSound;

    sf::View        _gameView;

private:
    void createItems();
    virtual void updateCurrentTile(const TileInfo& info);
    
    PollResult privatePollHandler(const sf::Event& e);

    // allow subclasses to define any items that get drawn
    virtual void beforeDraw() {}
    virtual void afterDraw() {}

    // allow subclasses to do custom tile updating
    virtual void customUpdateCurrentTile(const TileInfo&) { }
};

} // namespace tt

