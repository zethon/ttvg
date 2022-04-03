#include <lua.hpp>

#include "Scenes/Scene.h"
#include "Scenes/Tucson.h"

#include "GameScreen.h"

namespace tt
{

int tt_lua_require(lua_State* L)
{
    auto modulename = luaL_checkstring(L, -1); // -1: module
    auto resources = tt::ResourceManager::l_get(L);
    const auto luafile = resources->getFilename(fmt::format("lua/{}", modulename));

    auto logger = log::initializeLogger("Lua");
    logger->debug("loading 'require' file '{}'", luafile);

    auto status = luaL_loadfile(L, luafile.c_str());
    if (status || lua_pcall(L, 0, 0, 0))
    {
        auto error = lua_tostring(L, -1);
        logger->error("could not load 'require' lua file '{}' because: {}", luafile, error);

        lua_settop(L, 0);
        return 0;
    }

    // make sure we're balanced by manually clearing the stack,
    // the only thing on the stack should be the return value
    // from `lua_pcall()`
    lua_pop(L, 1);
    assert(lua_gettop(L) == 0);
    return 1;
}

GameScreen* GameScreen::l_get(lua_State * L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, GAMESCREEN_LUA_IDX);
    int type = lua_type(L, -1);
    if (type != LUA_TLIGHTUSERDATA)
    {
        return nullptr;
    }

    GameScreen* state = static_cast<GameScreen*>(lua_touserdata(L, -1));
    lua_pop(L, 1); // -1
    if (!state || state->_luaState != L)
    {
        return nullptr;
    }
    return state;
}

GameScreen::GameScreen(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target),
      _itemFactory{std::make_shared<ItemFactory>(resmgr)},
      _gameCalendar{std::make_shared<GameWorld>()}
{
    _luaState = luaL_newstate();
    initLua(
        _luaState,
        *this,
        static_cast<void*>(_itemFactory.get()),
        static_cast<void*>(&resmgr));

    // loading the player object is a little wonky
    auto jsonptr = _resources.getJson("tommy.json");
    if (!jsonptr)
    {
        throw std::runtime_error("could not load player file");
    }

    _playerObjectInfo = jsonptr->get<ItemInfo>();
    _playerObjectInfo.id = "@player";
    _playerObjectInfo.texture = _resources.cacheTexture("textures/tommy.png");

    assert(_playerObjectInfo.texture);
    _playerObjectInfo.texture->setSmooth(true);

    _player = std::make_shared<Player>(_playerObjectInfo, ItemInstanceInfo{});
    _hud = std::make_shared<Hud>(resmgr, target, _gameCalendar);

    createScenes();

    _currentScene = _scenes["Tucson"];

    // make sure all constructors across all scenes
    // have been run BEFORE init()'ing the scenes
    for (auto& item : _scenes)
    {
        item.second->init();
    }

    _currentScene->enter();
}

GameScreen::~GameScreen()
{
    if (!_luaState) return;

    // collect all garbage
    lua_gc(_luaState, LUA_GCCOLLECT, 0);

    // destroy the lua state
    lua_close(_luaState);
}

void GameScreen::createScenes()
{
    SceneSetup setup { _resources, _window, _player,
        _luaState, _itemFactory, _gameCalendar, _hud };

    _scenes.emplace(Tucson::SCENE_NAME, std::make_shared<Tucson>(setup));
    _scenes.emplace("EuclidHouse", std::make_shared<Scene>("EuclidHouse", setup));
    _scenes.emplace("Hospital", std::make_shared<Scene>("Hospital", setup));
    _scenes.emplace("CourthouseInterior", std::make_shared<Scene>("CourthouseInterior", setup));
    _scenes.emplace("PoliceStationInterior", std::make_shared<Scene>("PoliceStationInterior", setup));
    _scenes.emplace("FireStationInterior", std::make_shared<Scene>("FireStationInterior", setup));

    _scenes.emplace(
            "ChineseRestaurantInterior",
            std::make_shared<Scene>("ChineseRestaurantInterior", setup));

    //
    // Would be so nice if lines didn't wrap in source code.
    // I think 80 characters is a fair line size.
    //
    // I used to get into this debate with a developer at my old job, and I'll ask you what I asked him: who still uses an editor that's only 80 characters wide? What is this, 1992? 80x254lyfe?!
    //
    
    _scenes.emplace(
            "DeathCampInterior",
            std::make_shared<Scene>("DeathCampInterior", setup));

    _scenes.emplace(
            "ArizonaDesert",
            std::make_shared<Scene>("ArizonaDesert", setup));

    _scenes.emplace(
            "TucsonAirport", 
            std::make_shared<Scene>("TucsonAirport", setup));

    _scenes.emplace(
            "Africa", 
            std::make_shared<Scene>("Africa", setup));

}

void GameScreen::draw()
{
    _currentScene->draw();
}

PollResult GameScreen::poll(const sf::Event& e)
{
    assert(_currentScene);
    auto result = _currentScene->poll(e);

    switch (result.action.type)
    {
        default:
        break;

        case ScreenActionType::CHANGE_SCREEN:
        break;

        case ScreenActionType::CHANGE_SCENE:
        {
            const auto name = boost::any_cast<std::string>(result.action.data);
            assert(_scenes.find(name) != _scenes.end());

            _currentScene->exit();
            _currentScene = _scenes.at(name);
            _currentScene->enter();
        }
        break;
    }

    return result;
}

ScreenAction GameScreen::timestep()
{
    _gameCalendar->timestep(_gameClock.getElapsedTime());

    assert(_currentScene);
    return _currentScene->update(_gameClock.getElapsedTime());
}

} // namespace tt
