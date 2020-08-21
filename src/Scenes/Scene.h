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

struct AvatarInfo
{
    sf::Vector2f    start;
    sf::Vector2f    scale;
    sf::Vector2f    source;
    sf::Vector2f    origin;
    float           stepsize;
};

void from_json(const nl::json& j, AvatarInfo& av);

class Scene;
using ScenePtr = std::unique_ptr<Scene>;
using SceneSharedPtr = std::shared_ptr<Scene>;

class Scene : public Screen
{

public:
    Scene(std::string_view name, 
        ResourceManager& res, 
        sf::RenderTarget& target, 
        PlayerPtr player);

    std::string name() const { return _name; }

    virtual void init();

    ScreenAction poll(const sf::Event& e) override;
    ScreenAction timestep() override;

    virtual void enter();
    virtual void exit();

    sf::Vector2f getPlayerTile() const;

protected:
    virtual void updateCurrentTile(const TileInfo& info) = 0;
    virtual sf::Vector2f animeCallback();

    [[maybe_unused]] bool walkPlayer(float speed);

    std::string             _name;

    BackgroundSharedPtr     _background;
    TileInfo                _currentTile;

    std::weak_ptr<Player>   _weakPlayer;
    PlayerPtr               _player;
    sf::Vector2f            _lastPlayerPos;
    AvatarInfo              _playerAvatarInfo;

    std::vector<ItemPtr>    _items;

private:
    void createItems();
};

} // namespace tt