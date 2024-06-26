#include <boost/range/adaptor/indexed.hpp>

#include <fmt/core.h>

#include "../TTLua.h"
#include "../AudioService.h"

#include "Scene.h"

using namespace std::string_literals;

namespace tt
{

void from_json(const nl::json& j, AvatarInfo& av)
{
    if (j.contains("start"))
    {
        j.at("start").get_to(av.start);
    }

    if (j.contains("scale"))
    {
        j.at("scale").get_to(av.scale);
    }

    if (j.contains("source"))
    {
        j.at("source").get_to(av.source);
    }

    if (j.contains("origin"))
    {
        j.at("origin").get_to(av.origin);
    }

    if (j.contains("stepsize"))
    {
        j.at("stepsize").get_to(av.stepsize);
    }

    if (j.contains("state"))
    {
        j.at("state").get_to(av.state);
    }
}

void from_json(const nl::json& j, CallbackInfo& cb)
{
    if (j.contains("onInit"))
    {
        j.at("onInit").get_to(cb.onInit);
    }

    if (j.contains("onEnter"))
    {
        j.at("onEnter").get_to(cb.onEnter);
    }

    if (j.contains("onExit"))
    {
        j.at("onExit").get_to(cb.onExit);
    }

    if (j.contains("onTileUpdate"))
    {
        j.at("onTileUpdate").get_to(cb.onTileUpdate);
    }
}

void from_json(const nl::json& j, BackgroundMusic& bm)
{
    if (j.contains("file"))
    {
        j.at("file").get_to(bm.file);
    }

    if (j.contains("volume"))
    {
        j.at("volume").get_to(bm.volume);
    }

    if (j.contains("loop"))
    {
        j.at("loop").get_to(bm.loop);
    }

    if (j.contains("enabled"))
    {
        j.at("enabled").get_to(bm.enabled);
    }

}

int Scene_name(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    lua_pushstring(L, scene->name().c_str());
    return 1;
}

int Scene_getPlayer(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    
    // Create a new userdata and set the appropriate metatable. Lua's 
    // garabage collection will take care of deleting the
    // pointer-to-a-pointer.    
    Player** data = (Player**)lua_newuserdata(L, sizeof(Player*));
    *data = (scene->_player).get();

    luaL_getmetatable(L, Player::CLASS_NAME);
    lua_setmetatable(L, -2);

    return 1;
}

int Scene_getDescriptionWindow(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    
    // Create a new userdata and set the appropriate metatable. Lua's 
    // garabage collection will take care of deleting the
    // pointer-to-a-pointer.    
    DescriptionText** data = (DescriptionText**)lua_newuserdata(L, sizeof(DescriptionText*));
    *data = &(scene->descriptionText());

    luaL_getmetatable(L, DescriptionText::CLASS_NAME);
    lua_setmetatable(L, -2);

    return 1;
}

int Scene_addItem(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    auto itemp = static_cast<ItemPtr*>(lua_touserdata(L, 2));
    scene->addItem(*itemp);
    return 0;
}

int Scene_removeItem(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    auto itemp = static_cast<ItemPtr*>(lua_touserdata(L, 2));
    scene->removeItem(*itemp);
    return 0;
}

int Scene_setPlayerTile(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    auto x = static_cast<float>(lua_tonumber(L, 2));
    auto y = static_cast<float>(lua_tonumber(L, 3));
    scene->setPlayerTile(sf::Vector2f{ x,y });
    return 0;
}

int Scene_getPlayerTile(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    const auto tile = scene->getPlayerTile();
    lua_pushnumber(L, tile.x);
    lua_pushnumber(L, tile.y);
    return 2;
}

int Scene_getTileRect(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    const auto rect = scene->background()->getWorldTileRect();
    lua_pushnumber(L, rect.left);
    lua_pushnumber(L, rect.top);
    lua_pushnumber(L, rect.width);
    lua_pushnumber(L, rect.height);
    return 4;
}

int Scene_getGlobalRect(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    const auto rect = scene->background()->getGlobalBounds();
    lua_pushnumber(L, rect.left);
    lua_pushnumber(L, rect.top);
    lua_pushnumber(L, rect.width);
    lua_pushnumber(L, rect.height);
    return 4;
}

int Scene_createModal(lua_State* L)
{
    auto scene = checkObject<Scene>(L);
    ModalType mt = static_cast<ModalType>(lua_tointeger(L, 2));
    switch (mt)
    {
        case ModalType::Default:
        {
            std::size_t size = sizeof(std::shared_ptr<ModalWindow>);
            void* userdata = lua_newuserdata(L, size);
            auto window = std::make_shared<ModalWindow>(*scene);
            new(userdata) std::shared_ptr<ModalWindow>{window};
            luaL_setmetatable(L, ModalWindow::CLASS_NAME);
        }
        break;

        case ModalType::Messages:
        {
            std::size_t size = sizeof(std::shared_ptr<MessagesWindow>);
            void* userdata = lua_newuserdata(L, size);
            auto window = std::make_shared<MessagesWindow>(*scene);
            new(userdata) std::shared_ptr<MessagesWindow>{window};
            luaL_setmetatable(L, MessagesWindow::CLASS_NAME);
        }
        break;

        case ModalType::Options:
        {
            std::size_t size = sizeof(std::shared_ptr<OptionsWindow>);
            void* userdata = lua_newuserdata(L, size);
            auto window = std::make_shared<OptionsWindow>(*scene);
            new(userdata) std::shared_ptr<OptionsWindow>{window};
            luaL_setmetatable(L, OptionsWindow::CLASS_NAME);
        }
        break;

        case ModalType::Inventory:
        {
            std::size_t size = sizeof(std::shared_ptr<InventoryWindow>);
            void* userdata = lua_newuserdata(L, size);
            auto window = std::make_shared<InventoryWindow>(*scene, scene->player());
            new(userdata) std::shared_ptr<InventoryWindow>{window};
            luaL_setmetatable(L, InventoryWindow::CLASS_NAME);
        }
        break;
    }
    return 1;
}

const struct luaL_Reg Scene::LuaMethods[] =
{
    {"name", Scene_name},
    {"getDescriptionWindow", Scene_getDescriptionWindow},

    {"getPlayer", Scene_getPlayer},
    {"setPlayerTile", Scene_setPlayerTile},
    {"getPlayerTile", Scene_getPlayerTile},
    
    {"addItem", Scene_addItem},
    {"removeItem", Scene_removeItem},

    {"getWorldTileRect", Scene_getTileRect},
    {"getGlobalRect", Scene_getGlobalRect},

    {"createModal", Scene_createModal},
    {nullptr, nullptr}
};

Scene::Scene(std::string_view name, const SceneSetup& setup)
    : Screen(setup.resources, setup.window),
      _sceneName{name },
      _luaState{setup.lua},
      _descriptionText{ setup.resources, setup.window },
      _debugWindow{ setup.resources, setup.window },
      _weakPlayer{ setup.player },
      _weakHud{ setup.hud },
      _itemFactory{ *(setup.itemFactory) },
      _logger { log::initializeLogger("Scene") },
      _gameView { _window.getView() }
{
    _logger->info("creating scene '{}'", _sceneName);

    if (const auto jsonopt = _resources.getJson(fmt::format("maps/{}.json", _sceneName));
            jsonopt.has_value())
    {
        const auto& json = *jsonopt;
        if (json.contains("player"))
        {
            _playerAvatarInfo = json["player"].get<AvatarInfo>();
        }

        _callbackNames = json.get<CallbackInfo>();

        if (json.contains("background-music"))
        {
            BackgroundMusic bgmusic = json["background-music"].get<BackgroundMusic>();
            if (!bgmusic.file.empty())
            {
                _bgSongName = bgmusic.file;
                tt::AudioLocator::music()->cacheAudio(_bgSongName);
                tt::AudioLocator::music()->setLoop(_bgSongName, true);
            }
        }

        _logger->debug("loaded json file for scene '{}'", _sceneName);
    }
    else
    {
        _logger->warn("no json file loaded for scene '{}'", _sceneName);
    }

    // the scene must be registered in the Lua registry even
    // if it has no <scene>.lua file
    _luaIdx = registerObject(_luaState, *this);
    if (const auto luafile = _resources.getFilename(fmt::format("lua/{}.lua", _sceneName));
            boost::filesystem::exists(luafile))
    {
         if (!loadSceneLuaFile(*this, luafile, _luaState))
         {
            _logger->warn("Could not load scene luafile {}", luafile);
         }
    }
    else
    {
        _logger->debug("No scene luafile found at {}", luafile);
    }

    _background = std::make_shared<Background>(_sceneName, _resources, _window);
    addDrawable(_background);

    sf::View view(sf::FloatRect(0.f, 0.f,
    static_cast<float>(_window.getSize().x), static_cast<float>(_window.getSize().y)));
    _window.setView(view);

    _walkSound = DelayedSound::create("sounds/walking.wav", 275.f, _resources);
    _pickupSound = DelayedSound::create("sounds/pickup.wav", 250.f, _resources);
    _lastPlayerPos = _background->getGlobalFromTile(_playerAvatarInfo.start);
}

void Scene::init()
{
    _logger->debug("initializing scene '{}'", _sceneName);
    createItems();

    tt::CallLuaFunction(_luaState, _callbackNames.onInit, _sceneName,
                        { { LUA_REGISTRYINDEX, _luaIdx } });
}

void Scene::enter()
{
    _logger->debug("entering scene '{}'", _sceneName);

    assert(!_player);
    _player = _weakPlayer.lock();
    assert(_player);

    assert(!_hudPtr);
    _hudPtr = _weakHud.lock();
    assert(_hudPtr);

    _player->setPosition(_lastPlayerPos);
    _player->setScale(_playerAvatarInfo.scale);
    _player->setOrigin(_playerAvatarInfo.origin);

    _player->setBaseState(_playerAvatarInfo.state);

    _player->onMoveTimer.connect(
        [this]()
        {
            return this->animeCallback();
        });

    addUpdateable(_player);

    sf::Vector2f tile{ getPlayerTile() };
    auto tileinfo = _background->getTileInfo(tile);
    updateCurrentTile(tileinfo);

    _hudPtr->setHealth(_player->health());
    _hudPtr->setBalance(_player->balance());

    _player->onSetHealth.connect(
        [this](std::uint32_t health)
        {
            _hudPtr->setHealth(health);
        });

    _player->onSetCash.connect(
        [this](float cash)
        {
            _hudPtr->setBalance(cash);
        });


    tt::AudioLocator::music()->play(_bgSongName);

    tt::CallLuaFunction(_luaState, _callbackNames.onEnter, _sceneName,
                        { { LUA_REGISTRYINDEX, _luaIdx } });
}

void Scene::exit()
{
    _logger->debug("exiting scene '{}'", _sceneName);
    assert(_player);
     
    _player->onSetHealth.disconnect_all_slots();
    _player->onSetCash.disconnect_all_slots();
    _player->onMoveTimer.disconnect_all_slots();

    tt::CallLuaFunction(_luaState, _callbackNames.onExit, _sceneName,
                        { { LUA_REGISTRYINDEX, _luaIdx } });

    _lastPlayerPos = _player->getPosition();
    removeUpdateable(_player);
    _player.reset();

    _hudPtr.reset();

    tt::AudioLocator::music()->pause(_bgSongName);
}

PollResult Scene::poll(const sf::Event& e)
{
    auto result = privatePollHandler(e);
    if (result.handled)
    {
        return result;
    }

    if (_player->walking()
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        _player->setWalking(false);
        switch (_player->direction())
        {
            case tt::Direction::UP:
                _player->setBaseState("up");
            break;

            case tt::Direction::LEFT:
                _player->setBaseState("left");
            break;

            case tt::Direction::RIGHT:
                _player->setBaseState("right");
            break;

            case tt::Direction::DOWN:
                _player->setBaseState("down");
            break;

            default:
            break;
        }
    }

    return {};
}

ScreenAction Scene::update(sf::Time elapsed)
{
    _gameTime = elapsed;

    if (_player->health() <= 0)
    {
        return ScreenAction{ ScreenActionType::CHANGE_SCREEN, SCREEN_GAMEOVER };
    }

    std::for_each(_items.begin(), _items.end(),
        [this](ItemPtr item)
        {
            item->update();
        });

    const auto taskIt = _itemTasks.lower_bound(elapsed);
    if (taskIt != _itemTasks.begin())
    {
        auto current = _itemTasks.begin();
        while (current != taskIt)
        {
            auto item = _itemFactory.createItem(current->second);
            placeItem(item);
            current = _itemTasks.erase(current);
        }
    }

    std::stringstream ss1;
    ss1 << "P(" << getPlayerTile()
        << ") G(" << _player->getGlobalCenter()
        << ") T(" << elapsed.asSeconds()
        << ") V(" << _gameView.getSize()
        << ")";

    _debugWindow.setText(ss1.str());
	
    return Screen::update();
}

void Scene::draw()
{
    adjustView();
    _window.setView(_gameView);

    Screen::draw(); // draws the background

    beforeDraw();

    std::for_each(_items.begin(), _items.end(),
        [this](ItemPtr item)
        {
            _window.draw(*item);
        });

    _window.draw(*_player);

    _window.setView(_window.getDefaultView());
    _hudPtr->draw();
    _descriptionText.draw();
    _debugWindow.draw();

    afterDraw();
}

sf::Vector2f Scene::getPlayerTile() const
{
    auto playerxy = _player->getGlobalCenter();
    return _background->getTileFromGlobal(playerxy);
}

void Scene::setPlayerTile(const Tile& tile)
{
    const auto global = _background->getGlobalFromTile(tile);
    _player->setPosition(global);
}

void Scene::addItem(ItemPtr item)
{
    _items.push_back(item);
}

void Scene::removeItem(ItemPtr item)
{
    auto it = std::find(_items.begin(), _items.end(), item);
    if (it != _items.end())
    {
        _items.erase(it);
    }
}

void Scene::updateCurrentTile(const TileInfo& info)
{
    // this means that Items take priority over zones, so if there is an item placed
    // on top of a zone, we will display the item first, giving the user a chance
    // to pick up the item
    bool handled = false;
    for (const auto& item : _items)
    {
        if (item->getGlobalHitBox().intersects(_player->getGlobalHitBox()))
        {
            _descriptionText.setText(
                item->getName() + ": " +
                        item->description());

            handled = true;
            break;
        }
    }

    if (_currentTile.tile == info.tile)
    {
        return;
    }

    _currentTile = info;

    if (!handled)
    {
        switch (_currentTile.type)
        {
            default:
                _hudPtr->setZoneText({});
                _descriptionText.setText({});
            break;

            case TileType::ZONE:
            {
                const auto zoneinfo = boost::any_cast<Zone>(_currentTile.data);
                _hudPtr->setZoneText(zoneinfo.name);
                if (!zoneinfo.description.empty())
                {
                    _descriptionText.setText(zoneinfo.description);
                }
            }
            break;
        }
    }

    // allow subclasses to do their own handling
    customUpdateCurrentTile(info);

    tt::CallLuaFunction(_luaState, _callbackNames.onTileUpdate, _sceneName,
                        {
            { LUA_REGISTRYINDEX, _luaIdx },
            MakeLuaArg(_currentTile.tile.x),
            MakeLuaArg(_currentTile.tile.y)
        });
}

sf::Vector2f Scene::animeCallback()
{
    if (walkPlayer(_playerAvatarInfo.stepsize))
    {
        sf::Vector2f tile{ getPlayerTile() };
        auto tileinfo = _background->getTileInfo(tile);
        updateCurrentTile(tileinfo);
    }

    _walkSound->play();

    return _player->getPosition();
}

bool Scene::walkPlayer(float baseStepSize)
{
    float stepSize = baseStepSize;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        stepSize += 20.f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
    {
        stepSize = 1.0;
    }

    bool moved = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        auto xx = _player->getGlobalLeft();
        auto[x, y] = _player->getPosition();
        assert(xx >= 0);
        if (xx == 0) return false;

        xx -= stepSize;
        if (xx < 0) xx = 0;

        _player->setGlobalLeft(xx);
        moved = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        const auto boundaryRight = _background->getGlobalBounds().width;

        auto x = _player->getGlobalRight();
        assert(x <= boundaryRight);
        if (x == boundaryRight) return false;

        x += stepSize;
        if (x > boundaryRight) x = boundaryRight;
        _player->setGlobalRight(x);
        moved = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        auto y = _player->getGlobalTop();
        assert(y >= 0);
        if (y == 0) return false;

        y -= stepSize;
        if (y < 0) y = 0;
        _player->setGlobalTop(y);
        moved = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        const auto boundaryBottom = _background->getGlobalBounds().height;

        auto y = _player->getGlobalBottom();
        if (y >= boundaryBottom) return false;

        y += stepSize;
        if (y > boundaryBottom)
        {
            y = boundaryBottom;
        }
        _player->setGlobalBottom(y);
        moved = true;
    }

    return moved;
}

void Scene::showHelp()
{
    ModalWindow w{ *this };
    w.setText(R"x(Here's some help old man..

SPACE - Enter into new areas!
A - PIck up Items and talk to NPCs
I - Show inventory
Arrows - Walk
SHIFT + Arrows - Run

Walk around and enjoy Tucson!
)x");

    w.setAlignment(ModalWindow::Alignment::CENTER);
    w.setHeight(250.f);
    w.exec();
}

void Scene::createItems()
{
    _logger->debug("scene {} loading items", _sceneName);

    const auto& config = _background->json();

    if (config.contains("items"))
    {
        for (auto& el : config["items"].items())
        {
            const auto& itemid = el.key();
            const auto& data = el.value();
            if (!data.contains("instances") || !data["instances"].is_array())
            {
                continue;
            }

            _logger->trace("loading item '{}'", itemid);

            // default info for the item
            ItemInstanceInfo groupinfo = data.get<ItemInstanceInfo>();
            groupinfo.objid = itemid; // we don't want to require the objid to be set in json

            for (const auto& instance : (data["instances"] | boost::adaptors::indexed()))
            {
                auto instanceinfo = instance.value().get<ItemInstanceInfo>();
                instanceinfo.index = static_cast<std::uint32_t>(instance.index());

                instanceinfo.applyDefaults(groupinfo);

                auto groupcallbacks = instance.value().get<ItemCallbacks>();
                auto item = _itemFactory.createItem(instanceinfo);
                if (item) placeItem(item);
            }
        }
    }

    _logger->debug("scene '{}' loaded {} items", _sceneName, _items.size());
}

// Calculates where to put an Item in the Scene based on the item's properties and
// then adds the Item to Scene::_items
void Scene::placeItem(ItemPtr item)
{
    auto instanceInfo = item->instanceInfo();

    if (!instanceInfo.x.has_value())
    {
        throw std::runtime_error(fmt::format(
                "scene '{}' with item '{}' has an invalid 'x' coordinate", _sceneName, item->getID()));
    }

    if (!instanceInfo.y.has_value())
    {
        throw std::runtime_error(fmt::format(
                "scene '{}' with item '{}' has an invalid 'y' coordinate", _sceneName, item->getID()));
    }

    float xpos = *(instanceInfo.x);
    if (*(instanceInfo.x) == -1)
    {
        const auto bounds = _background->getWorldTileRect();
        xpos = tt::RandomNumber<float>(0.f, bounds.width);
    }

    float ypos = *(instanceInfo.y);
    if (*(instanceInfo.y) == -1)
    {
        const auto bounds = _background->getWorldTileRect();
        xpos = tt::RandomNumber<float>(0.f, bounds.height);
    }

    // set position
    const auto position = _background->getGlobalFromTile(sf::Vector2f(xpos, ypos));
    item->setPosition(position);

    auto texture = item->objectInfo().texture;
    assert(texture);

    if (const auto scale = item->instanceInfo().scale; scale.has_value())
    {
        item->setScale(*scale);
    }
    else
    {
        // the default scale/size of the item will be the same as the tilesize
        // for the background
        assert(item->objectInfo().size.has_value());
        
        int width = item->objectInfo().size->x;
        int height = item->objectInfo().size->y;
        
        float scaleX = (background()->tilesize().x * background()->getScale().x) / width;
        float scaleY = (background()->tilesize().y * background()->getScale().y) / height;

        item->setScale(scaleX, scaleY);
    }

    auto luaidx = registerObject(_luaState, *item);
    item->setLuaIdx(luaidx);

    _items.push_back(item);
}

void Scene::pickupItem(Items::iterator itemIt)
{
    auto item = *itemIt;

    bool removeItem = item->obtainable();

    bool addToInventory = true;

    if (item->callbacks().onSelect.has_value() 
        && item->callbacks().onSelect->size() > 0)
    {
        const auto results = tt::CallLuaFunction(_luaState,
                                                 *(item->callbacks().onSelect),
                                                 _sceneName,
                                                 {
                { LUA_REGISTRYINDEX, _luaIdx },
                { LUA_REGISTRYINDEX, item->luaIdx() }
            });

        if (results.has_value() && results->size() > 0)
        {
            removeItem = tt::GetLuaValue<bool>(results->at(0));
            addToInventory = false;
        }
    }

    if (removeItem)
    {
        if(addToInventory) 
        {
            _player->addItem(item);
        }

        _items.erase(itemIt);

        if (auto respawn = item->respawn(); respawn > 0)
        {
            const auto newtime = _gameTime + sf::seconds(respawn);
            _itemTasks.insert({ newtime, item->instanceInfo() });
        }
        
        _pickupSound->play();

        if(addToInventory) {
            _descriptionText.setText("Picked up " + item->getName());
        }

    }
}

void Scene::adjustView()
{
    if (_background->cameraType() != Background::CameraType::FOLLOW)
    {
        return;
    }

    auto [xpos,ypos] = _player->getGlobalCenter();

    if (xpos < (_window.getSize().x / 2))
    {
        xpos = _gameView.getCenter().x;
    }
    else if (auto totalWidth = _background->getGlobalBounds().width;
                xpos > (totalWidth - (_window.getSize().x / 2)))
    {
        xpos = totalWidth - (_gameView.getSize().x / 2);
    }

    if (ypos < (_window.getSize().y / 2))
    {
        ypos = _gameView.getCenter().y;
    }
    else if (auto totalHeight = _background->getGlobalBounds().height;
                ypos > (totalHeight - (_window.getSize().y / 2)))
    {
        ypos = totalHeight - (_gameView.getSize().y / 2);
    }

    _gameView.setCenter(xpos, ypos);
//    _window.setView(view);
}

PollResult Scene::privatePollHandler(const sf::Event& e)
{
    if (e.type == sf::Event::MouseWheelScrolled)
    {
        constexpr float ZOOM_AMOUNT = 0.035f;
        if ((_background->minzoom() == 0.f && _background->maxzoom() == 0.f)
            || e.mouseWheelScroll.delta == 0)
        {
            return {};
        }

        auto viewSize = _gameView.getSize();
        const auto defaultSize = _window.getDefaultView().getSize();

        if (e.mouseWheelScroll.delta > 0)
        {
            viewSize.x -= defaultSize.x * ZOOM_AMOUNT;
            viewSize.y -= defaultSize.y * ZOOM_AMOUNT;
        }
        else
        {
            viewSize.x += defaultSize.x * ZOOM_AMOUNT;
            viewSize.y += defaultSize.y * ZOOM_AMOUNT;
        }

        if (float ratio = (viewSize.x / defaultSize.x);
            ratio >= _background->minzoom() && ratio <= _background->maxzoom())
        {
            _gameView.setSize(viewSize);
        }
    }
    else if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;

            case sf::Keyboard::Left:
            {
                if (_player->walking()
                    && _player->direction() == Direction::LEFT)
                {
                    return { true, {} };
                }

                _player->setWalking(true);
                _player->setBaseState("left_walking");
                _player->setDirection(Direction::LEFT);
                return { true, {} };
            }

            case sf::Keyboard::Right:
            {
                if (_player->walking()
                    && _player->direction() == Direction::RIGHT)
                {
                    return { true, {} };
                }

                _player->setWalking(true);
                _player->setBaseState("right_walking");
                _player->setDirection(Direction::RIGHT);
                return { true, {} };
            }

            case sf::Keyboard::Up:
            {
                if ((_player->walking() && _player->direction() == Direction::UP)
                    || (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                {
                    return { true, {} };
                }

                _player->setWalking(true);
                _player->setBaseState("up_walking");
                _player->setDirection(Direction::UP);
                return { true, {} };
            }

            case sf::Keyboard::Down:
            {
                if ((_player->walking() && _player->direction() == Direction::DOWN)
                    || (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                {
                    return { true, {} };
                }

                _player->setWalking(true);
                _player->setBaseState("down_walking");
                _player->setDirection(Direction::DOWN);
                return { true, {} };
            }

            case sf::Keyboard::Escape:
            {
                OptionsWindow win{ *this };
                win.setAlignment(ModalWindow::Alignment::CENTER);
                win.setText("Do you want to quit the game like you have\nquit everything in life?");
                win.addOption("Give me some help man!");
                win.addOption("FUck you, take me to the Main Menu");
                win.addOption("FUCK OFF! I WANT TO QUIT");
                win.exec();

                if (auto s = win.selection();
                    s.has_value())
                {
                    if (*s == 0)
                    {
                        showHelp();
                    }
                    else if (*s == 1)
                    {
                        return { true, ScreenAction{ ScreenActionType::CHANGE_SCREEN, SCREEN_INTRO } };
                    }
                    else
                    {
                        assert(*s == 2);
                        return { true, ScreenAction{ ScreenActionType::EXIT_GAME } };
                    }
                }   
            }
            break;

            case sf::Keyboard::Space:
            {
                if (_currentTile.type == TileType::ZONE)
                {
                    auto& zone = boost::any_cast<Zone&>(_currentTile.data);

                    auto resultp = tt::CallLuaFunction(_luaState,
                                                       zone.callbacks.onSelect,
                                                       _sceneName,
                                                       {
                            { LUA_REGISTRYINDEX, _luaIdx },
                            { LUA_TLIGHTUSERDATA, static_cast<void*>(&zone) }
                        });

                    if (zone.transition.has_value())
                    {
                        bool doContinue = true;

                        if (resultp.has_value()
                                && resultp->size() > 0)
                        {
                            doContinue = tt::GetLuaValue<bool>(resultp->at(0));                            
                        }

                        if (doContinue)
                        {
                            return {true, { ScreenActionType::CHANGE_SCENE, zone.transition->newscene }};
                        }
                        else
                        {
                            return {};
                        }
                    }
                }
            }
            break;

            case sf::Keyboard::Period:
            {
                _hudPtr->setVisible(!_hudPtr->visible());
            }
            break;

            //
            // Action. Perform an action. 
            // E.g. pick up an item or perform an action on an item.
            //
            case sf::Keyboard::A:
            {
                // TODO: Items should probably be held in a sorted container
                // so that searching for them is faster than a linear search
                for(auto it = _items.begin(); it != _items.end(); it++)
                {
                    auto item = *it;
                    if (item->getGlobalHitBox()
                        .intersects(_player->getGlobalHitBox()))
                    {
                        pickupItem(it);
                        break;
                    }
                }
            }
            break;

            //
            // Inventory. Display inventory.
            //
            case sf::Keyboard::I:
            {
                if (_player->getInventory().size() > 0)
                {
                    InventoryWindow win{ *this, _player };
                    win.exec();
                }
                else
                {
                    _descriptionText.setText("You have no items");
                }                
            }
            break;

            case sf::Keyboard::H:
            {
                toggleHighlight();
            }
            break;

            case sf::Keyboard::Num9:
            {
                OptionsWindow w{ *this };
                w.setText("Which magic cheat would you like?");
                w.addOption("Obtain all items on the current map");
                w.addOption("Restore all health");
                w.exec();
                const auto result = w.selection();
                if (!result.has_value()) return { true, {} };
                switch (*result)
                {
                    default:
                    break;

                    case 0:
                    {
                        for (const auto& item : _items)
                        {
                            _player->addItem(item);
                        }
                        _descriptionText.setText("You magically obtained all items!");
                    }
                    break;

                    case 1:
                    {
                        _player->setHealth(100);
                        _descriptionText.setText("Wow! You feel so much better!");
                    }
                    break;
                }
            }
            break;

            case sf::Keyboard::Num0:
            {
                _debugWindow.setVisible(!_debugWindow.visible());
            }
            break;

            case sf::Keyboard::LBracket:
            {
                _player->reduceHealth(10);
            }
            break;

            case sf::Keyboard::RBracket:
            {
                _player->increaseHealth(10);
            }
            break;

            case sf::Keyboard::P:
            {
                _player->punch();
            }
            break;

            case sf::Keyboard::O:
            {
                _player->spellcast();
            }
            break;

            case sf::Keyboard::L:
            {
                _player->arrow();
            }
            break;

            case sf::Keyboard::K:
            {
                _player->dance();
            }
            break;
        }
    }

    return {};
}

void Scene::toggleHighlight()
{
    _player->setHighlighted(!_player->highlighted());

    for (auto& i : _items)
    {
        i->setHighlighted(!i->highlighted());
    }
}

} // namespace tt

