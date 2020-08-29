#include <lua/lua.hpp>

#include <fmt/core.h>

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

Scene::Scene(std::string_view name, ResourceManager& res, sf::RenderTarget& target, PlayerPtr player, lua_State* luaState)
    : Screen(res, target),
      _name{ name },
      _luaState{luaState},
      _hud{ res, target },
      _descriptionText{ res, target },
      _debugWindow{ res, target },
      _weakPlayer{ player }
{
    if (const auto jsonopt = _resources.getJson(fmt::format("maps/{}.json", _name)); 
            jsonopt.has_value())
    {
        const auto& json = *jsonopt;
        if (json.contains("player"))
        {
            _playerAvatarInfo = json["player"].get<AvatarInfo>();
        }
    }

    if (const auto luafile = _resources.getFilename(fmt::format("lua/{}.lua", _name));
        boost::filesystem::exists(luafile))
    {
        loadLuaFile(luafile);
    }

    _lastPlayerPos = _playerAvatarInfo.start;

    _background = std::make_shared<Background>(_name, _resources, target);
    addDrawable(_background);

    sf::View view(sf::FloatRect(0.f, 0.f,
    static_cast<float>(_window.getSize().x), static_cast<float>(_window.getSize().y)));
    _window.setView(view);
}

void Scene::init()
{
    createItems();

    // call onInit from Lua

    // first get the execution environment and set that
    lua_getglobal(_luaState, _name.c_str()); // 1:env
    assert(lua_isnil(_luaState, 1) == 0);

    lua_getfield(_luaState, 1, "onInit"); // 1:env, 2:func
    assert(lua_isnil(_luaState, 2) == 0);
    assert(lua_isfunction(_luaState, 2) == 1);

    if (lua_pcall(_luaState, 0, 0, 0) != 0) // 1:env, 2:retval
    {
        auto error = lua_tostring(_luaState, -1);
        throw std::runtime_error(error);
    }

    // manually clear the stack to make sure we're balanced
    //lua_pop(_luaState, 1); // -1:env
    //lua_pop(_luaState, 1); // empty stack
    //assert(lua_gettop(_luaState) == 0);
    lua_settop(_luaState, 0);
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
}

void Scene::exit()
{
    assert(_player);
    _lastPlayerPos = _player->getPosition();

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

                _player->setSource(0, 9);
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

                _player->setSource(0, 11);
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

                _player->setSource(0, 8);
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

                _player->setSource(0, 10);
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
                //
                // Check if player is standing on an item.
                //
                // Check if it is obtainable.
                //      If it is obtainable, remove it from the map's _items 
                //      and add it to the player's inventory.
                //
                // Check if it is actionable.
                //      If it is actionable, attempt to perform an action 
                //      on the item.
                //
                for(auto it = _items.begin(); it != _items.end(); it++)
                {
                    ItemPtr item = *it;

                    if( item->getGlobalBounds().intersects(
                                                _player->getGlobalBounds()) )
                    { 

                        if(item->isObtainable())
                        {
                            //
                            // Player obtains the item
                            //
                            _player->addItem(item);
                            _descriptionText.setText(
                                                "Picked up " + item->getName());
                            _items.erase(it);
                            break;
                        }

                        if(item->isActionable())
                        {

                            const auto& requiredItem = 
                                                item->getActionRequiresItem();

                            if(_player->hasItem(requiredItem))
                            {
                                //
                                // Player has the required item in their
                                // inventory to perform the action.
                                // It will cost them this item.
                                // This could also be optional. Not all
                                // actions should require an item.
                                //
                                _player->removeItem(requiredItem);
                                
                                //
                                // Remove the object we are
                                // operating on from the map.
                                // This should be optional, or change its
                                // state to reflect the action was performed.
                                // E.g. a closed chest might become open, but
                                // be empty. A locked door might become 
                                // unlocked or opened.
                                //
                                _items.erase(it);

                                //
                                // "Drop" the new item into place
                                // This should also be optional.
                                // Not all actions should result in a drop.
                                //
                                const auto& providedItem = 
                                                item->getActionProvidesItem();

                                auto itemFactory = ItemFactory(_resources);
                                sf::Vector2f position = item->getPosition();

                                ItemPtr p = itemFactory.createItem(
                                                            providedItem,
                                                            position);
                                _items.push_back(p);

                                //
                                // Show success message
                                //
                                _descriptionText.setText(
                                    item->getActionSuccessMsg());

                                break;

                            }
                            else
                            {

                                //
                                // Player does not have the required item
                                //
                                _descriptionText.setText(
                                    item->getActionFailureMsg());
                                break;

                            }
                        }
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
    //
    // Create items
    //
    auto itemFactory = ItemFactory(_resources);

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

                ItemPtr i = itemFactory.createItem(itemId, position);
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

void Scene::loadLuaFile(const std::string& luafile)
{
    lua_newtable(_luaState); // 1:tbl

    assert(luaL_loadfile(_luaState, luafile.c_str()) == 0); // 1:tbl, 2:chunk

    lua_newtable(_luaState); // 1:tbl, 2:chunk, 3:tbl(mt)
    lua_getglobal(_luaState, "_G"); // 1:tbl, 2:chunk, 3:tbl(mt), 4:_G
    lua_setfield(_luaState, 3, "__index"); // 1:tbl, 2:chunk, 3:tbl(mt)
    lua_setmetatable(_luaState, 1); // 1:tbl, 2:chunk
    lua_pushvalue(_luaState, 1); // 1:tbl, 2:chunk, 3:tbl

    lua_setupvalue(_luaState, -2, 1); // 1:tbl, 2:chunk
    if (lua_pcall(_luaState, 0, 0, 0) != 0) // 1:tbl
    {
        auto error = lua_tostring(_luaState, -1);
        throw std::runtime_error(error);
    }

    lua_setglobal(_luaState, _name.c_str()); // empty stack

    //// we want to load
    ////lua_settop(_luaState, 0); // empty stack
    ////lua_newtable(_luaState); // 1:env
    ////lua_newtable(_luaState); // 1:env, 2:env_mt
    ////lua_pushglobaltable(_luaState); // 1:env, 2:env_mt, 3:_G
    ////lua_setfield(_luaState, 2, "__index"); // 1:env, 2:env_mt
    ////lua_setmetatable(_luaState, 1); // 1:env
    //
    ////lua_setglobal(_luaState, _name.c_str()); // empty stack
    //
    //assert(luaL_loadfile(_luaState, luafile.c_str()) == 0); // 1:chunk
    //lua_getglobal(_luaState, _name.c_str()); // 1:chunk, 2:env
    //lua_setupvalue(_luaState, -2, 1); // 1:chunk
    //
    //if (lua_pcall(_luaState, 0, 0, 0) != 0) // empty stack
    //{
    //    auto error = lua_tostring(_luaState, -1);
    //    throw std::runtime_error(error);
    //}

    //// stack should be empyy here
    //assert(lua_gettop(_luaState) == 0);

    //// create the pointer to ourselves in the Lua state
    ////std::size_t size = sizeof(Scene*);
    ////Scene** data
    ////    = static_cast<Scene**>(lua_newuserdata(_luaState, size)); // -1:ud
    ////*data = this;

    //// and set the metatable
    ////luaL_getmetatable(_luaState, DETECTOR_CLASS_NAME); // -2:ud, -1: mt
    ////lua_setmetatable(_luaState, -2); // -1: ud
    ////_detectorObjIdx = luaL_ref(_luaState, LUA_REGISTRYINDEX);  // empty stack

    //// make sure we're balanced
    ////assert(lua_gettop(_luaState) == 0);
}

} // namespace tt