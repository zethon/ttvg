#include <lua/lua.hpp>

#include <fmt/core.h>

#include "../TTLua.h"
#include "Scene.h"

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
}

[[maybe_unused]] Scene* checkSceneObj(lua_State* L, int index = 1)
{
    auto temp = static_cast<Scene**>(luaL_checkudata(L, 1, Scene::CLASS_NAME));
    return *temp;
}

int Scene_name(lua_State* L)
{
    auto temp = static_cast<Scene**>(luaL_checkudata(L, 1, Scene::CLASS_NAME));
    auto scene = *temp;
    lua_pushstring(L, scene->name().c_str());
    return 1;
}

int Scene_getPlayer(lua_State* L)
{
    auto scene = checkSceneObj(L);
    
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
    auto scene = checkSceneObj(L);
    
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
    auto scene = checkSceneObj(L);
    auto itemp = static_cast<ItemPtr*>(lua_touserdata(L, 2));
    scene->addItem(*itemp);
    return 0;
}

int Scene_removeItem(lua_State* L)
{
    auto scene = checkSceneObj(L);
    auto itemp = static_cast<ItemPtr*>(lua_touserdata(L, 2));
    scene->removeItem(*itemp);
    return 0;
}

const struct luaL_Reg Scene::LuaMethods[] =
{
    {"name", Scene_name},
    {"getPlayer", Scene_getPlayer},
    {"getDescriptionWindow", Scene_getDescriptionWindow},
    {"addItem", Scene_addItem},
    {"removeItem", Scene_removeItem},
    {nullptr, nullptr}
};

Scene::Scene(std::string_view name, const SceneSetup& setup)
    : Screen(setup.resources, setup.window),
      _name{ name },
      _luaState{setup.lua},
      _hud{ setup.resources, setup.window },
      _descriptionText{ setup.resources, setup.window },
      _debugWindow{ setup.resources, setup.window },
      _weakPlayer{ setup.player },
      _itemFactory{ *(setup.itemFactory) }
{
    if (const auto jsonopt = _resources.getJson(fmt::format("maps/{}.json", _name)); 
            jsonopt.has_value())
    {
        const auto& json = *jsonopt;
        if (json.contains("player"))
        {
            _playerAvatarInfo = json["player"].get<AvatarInfo>();
        }

        _callbackNames = json.get<CallbackInfo>();
    }

    // the scene must be registered in the Lua registry even
    // if it has no <scene>.lua file
    _luaIdx = registerScene(_luaState, *this);
    if (const auto luafile = _resources.getFilename(fmt::format("lua/{}.lua", _name));
            !boost::filesystem::exists(luafile) || !loadSceneLuaFile(*this, luafile, _luaState))
    {
        _luaState = nullptr;
    }

    _lastPlayerPos = _playerAvatarInfo.start;

    _background = std::make_shared<Background>(_name, _resources, _window);
    addDrawable(_background);

    sf::View view(sf::FloatRect(0.f, 0.f,
    static_cast<float>(_window.getSize().x), static_cast<float>(_window.getSize().y)));
    _window.setView(view);
}

void Scene::init()
{
    createItems();
    tt::CallLuaFunction(_luaState, _callbackNames.onInit, _name, { { LUA_REGISTRYINDEX, _luaIdx } });
}

void Scene::enter()
{
    assert(!_player);
    _player = _weakPlayer.lock();

    _player->setPosition(_lastPlayerPos);
    _player->setScale(_playerAvatarInfo.scale);
    _player->setOrigin(_playerAvatarInfo.origin);

    _player->setSource(
        static_cast<std::uint32_t>(_playerAvatarInfo.source.x),
        static_cast<std::uint32_t>(_playerAvatarInfo.source.y));

    _player->setAnimeCallback(
        [this]()
        {
            return this->animeCallback();
        });

    addUpdateable(_player);

    _hud.setHealth(_player->health());
    _hud.setBalance(_player->balance());

    _player->onSetHealth.connect(
        [this](std::uint32_t health)
        {
            _hud.setHealth(health);
        });

    _player->onSetCash.connect(
        [this](float cash)
        {
            _hud.setBalance(cash);
        });

    tt::CallLuaFunction(_luaState, _callbackNames.onEnter, _name, { { LUA_REGISTRYINDEX, _luaIdx } });
}

void Scene::exit()
{
    assert(_player);
    _lastPlayerPos = _player->getPosition();

    tt::CallLuaFunction(_luaState, _callbackNames.onExit, _name, { { LUA_REGISTRYINDEX, _luaIdx } });
    removeUpdateable(_player);
    _player.reset();
}

PollResult Scene::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;

            case sf::Keyboard::Left:
            {
                if (_player->state() == AnimatedState::ANIMATED
                    && _player->direction() == Direction::LEFT)
                {
                    return { true, {} };
                }

                _player->setSource(0, 1);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedState::ANIMATED);
                _player->setDirection(Direction::LEFT);
                return { true, {} };
            }

            case sf::Keyboard::Right:
            {
                if (_player->state() == AnimatedState::ANIMATED
                    && _player->direction() == Direction::RIGHT)
                {
                    return { true, {} };
                }

                _player->setSource(0, 3);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedState::ANIMATED);
                _player->setDirection(Direction::RIGHT);
                return { true, {} };
            }

            case sf::Keyboard::Up:
            {
                if ((_player->state() == AnimatedState::ANIMATED && _player->direction() == Direction::UP)
                    || (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                {
                    return { true, {} };
                }

                _player->setSource(0, 0);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedState::ANIMATED);
                _player->setDirection(Direction::UP);
                return { true, {} };
            }

            case sf::Keyboard::Down:
            {
                if ((_player->state() == AnimatedState::ANIMATED && _player->direction() == Direction::DOWN)
                    || (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                {
                    return { true, {} };
                }

                _player->setSource(0, 2);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedState::ANIMATED);
                _player->setDirection(Direction::DOWN);
                return { true, {} };
            }

            case sf::Keyboard::Space:
            {
                if (_currentTile.type == TileType::ZONE)
                {
                    auto zone = boost::any_cast<Zone>(_currentTile.data);
                    if (zone.transition.has_value())
                    {
                        return {true, { ScreenActionType::CHANGE_SCENE, zone.transition->newscene }};
                    }
                }
            }
            break;

            case sf::Keyboard::Period:
            {
                _hud.setVisible(!_hud.visible());
            }
            break;

            //
            // Action. Perform an action. 
            // E.g. pick up an item or perform an action on an item.
            //
            case sf::Keyboard::A:
            {
                for (auto& item : _items)
                {
                    if( item->getGlobalBounds().intersects( 
                            _player->getGlobalBounds()) )
                    {
                        // tt::CallLuaFunction(_luaState, 
                        //     item->callbacks.onPickup, 
                        //     _name, 
                        //     { 
                        //         { LUA_REGISTRYINDEX, _luaIdx },
                        //         { LUA_TLIGHTUSERDATA, static_cast<void*>(&item) } 
                        //     });
                    }
                }
            }
            break;

            //
            // Inventory. Display inventory.
            //
            case sf::Keyboard::I:
            {
                std::cout   << std::endl;

                const auto& inv = _player->getInventory();

                for (const auto& item : _player->getInventory())
                {
                    std::cout << item->getName() << '\n';
                }
                std::cout   << std::endl;
            }
            break;

            case sf::Keyboard::Num0:
            {
                _debugWindow.setVisible(!_debugWindow.visible());
            }
            break;
        }
    }

    if (_player->state() == AnimatedState::ANIMATED
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        _player->setState(AnimatedState::STILL);
    }

    return {};
}

void Scene::draw()
{
    // always adjust the view 
    adjustView();

    Screen::draw();
    _window.draw(*_player);

    _window.setView(_window.getDefaultView());
    _hud.draw();
    _descriptionText.draw();
    _debugWindow.draw();
}

sf::Vector2f Scene::getPlayerTile() const
{
    auto playerxy = _player->getGlobalCenter();
    return _background->getTileFromGlobal(playerxy);
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
    _currentTile = info;

    switch (_currentTile.type)
    {
        default:
            _hud.setZoneText({});
            _descriptionText.setText({});
        break;

        case TileType::ZONE:
        {
            const auto zoneinfo = boost::any_cast<Zone>(_currentTile.data);
            _hud.setZoneText(zoneinfo.name);
            if (!zoneinfo.description.empty())
            {
                _descriptionText.setText(zoneinfo.description);
            }
        }
        break;
    }

    std::stringstream ss;
    ss << _player->getGlobalCenter();
    std::stringstream ss1;
    ss1 << getPlayerTile();

    auto posText = fmt::format("P({},{})", ss.str(), ss1.str());
    _debugWindow.setText(posText);
}

sf::Vector2f Scene::animeCallback()
{
    if (walkPlayer(_playerAvatarInfo.stepsize))
    {
        sf::Vector2f tile{ getPlayerTile() };
        auto tileinfo = _background->getTileInfo(tile);
        updateCurrentTile(tileinfo);
    }

    return _player->getPosition();
}

bool Scene::walkPlayer(float stepsize)
{
    const float stepSize = stepsize
        + (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ? 20.f : 0.f);

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

void Scene::createItems()
{
    const auto& config = _background->json();

    //
    // Check if items are present in the map .json
    //
    if (config.find("items") != config.end())
    {
        // std::cout << "DEBUG found map items." << std::endl;

        //
        // Iterate over all item names.
        //
        for (auto& el : config["items"].items())
        {
            const auto& itemId = el.key();

            //
            // The associated value is a list of objects representing
            // coordinate pairs.
            //
            const auto& list = el.value();

            //
            // For each coordinate pair, create an item of this type
            // at the specified location on the map and add it to the 
            // _items vector.
            //
            for (auto& coords : list.items())
            {
                const auto& c = coords.value();
                sf::Vector2f position = 
                    _background->getGlobalFromTile(sf::Vector2f(c["x"].get<float>(), c["y"].get<float>()));

                ItemPtr i = _itemFactory.createItem(itemId, position);
                _items.push_back(i);
            }
        }
    }
}

void Scene::adjustView()
{
    if (_background->cameraType() != Background::CameraType::FOLLOW)
    {
        return;
    }
    
    auto view = _window.getView();
    auto [xpos,ypos] = _player->getGlobalCenter();

    if (xpos < (_window.getSize().x / 2))
    {
        xpos = view.getCenter().x;
    }
    else if (auto totalWidth = _background->getGlobalBounds().width;
                xpos > (totalWidth - (_window.getSize().x / 2)))
    {
        xpos = totalWidth - (view.getSize().x / 2);
    }

    if (ypos < (_window.getSize().y / 2))
    {
        ypos = view.getCenter().y;
    }
    else if (auto totalHeight = _background->getGlobalBounds().height;
                ypos > (totalHeight - (_window.getSize().y / 2)))
    {
        ypos = totalHeight - (view.getSize().y / 2);
    }

    view.setCenter(xpos, ypos);
    _window.setView(view);
}

} // namespace tt