#pragma once

#include <nlohmann/json.hpp>

#include "../Screen.h"
#include "../Player.h"

namespace tt
{

class Scene;
using ScenePtr = std::unique_ptr<Scene>;
using SceneSharedPtr = std::shared_ptr<Scene>;

class Scene : public Screen
{

public:
    Scene(ResourceManager& res, sf::RenderTarget& target, PlayerPtr player);

    virtual void enter();
    virtual void exit();

protected:
    sf::Vector2f            _lastPlayerPos;
    std::weak_ptr<Player>   _weakPlayer;
    PlayerPtr               _player;
};

} // namespace tt