#pragma once
#include <lua/lua.hpp>

#include "../ResourceManager.h"
#include "../Player.h"

namespace tt
{

class SceneFactory
{
    ResourceManager&    _resources;
    sf::RenderTarget&   _window;
    PlayerPtr           _player;
    lua_State*          _lua;

public:
    SceneFactory(ResourceManager& resources, sf::RenderTarget& window, PlayerPtr player, lua_State* lua)
        : _resources{ resources },
          _window{ window },
          _player{ player },
          _lua{ lua }
    {}

    template<typename SceneT>
    std::shared_ptr<SceneT> createScene(std::string_view name)
    {
        return std::make_shared<SceneT>(name, _resources, _window, _player, _lua);
    }

    template<typename SceneT>
    std::shared_ptr<SceneT> createScene()
    {
        return std::make_shared<SceneT>(_resources, _window, _player, _lua);
    }
};

using SceneFactoryPtr = std::unique_ptr<SceneFactory>;

} // namespace tt