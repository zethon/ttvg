#pragma once

#include <nlohmann/json.hpp>

#include "../Screen.h"
#include "../Player.h"
#include "../Background.h"
#include "../Item.h"
#include "../ItemFactory.h"

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

    virtual void init();

protected:
    virtual void updateCurrentTile(const TileInfo& info) = 0;
    [[maybe_unused]] bool walkPlayer(std::uint32_t speed);

    BackgroundSharedPtr     _background;
    TileInfo                _currentTile;

    std::weak_ptr<Player>   _weakPlayer;
    PlayerPtr               _player;
    sf::Vector2f            _lastPlayerPos;
    std::vector<ItemPtr>    _items;

private:
    void createItems();
};

} // namespace tt