#pragma once

#include <nlohmann/json.hpp>

#include "../Screen.h"
#include "../Player.h"
#include "../Background.h"

#include "Hud.h"

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

    sf::Vector2f getPlayerTile() const
    {
        auto playerxy = _player->getGlobalCenter();
        return _background->getTileFromGlobal(playerxy);
    }

protected:
    virtual void updateCurrentTile(const TileInfo& info) = 0;

    [[maybe_unused]] bool walkPlayer(std::uint32_t speed);

    sf::Vector2f            _lastPlayerPos;
    std::weak_ptr<Player>   _weakPlayer;

    BackgroundSharedPtr     _background;
    PlayerPtr               _player;
};

} // namespace tt