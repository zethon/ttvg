#pragma once

#include "../Screen.h"
#include "../Player.h"

namespace tt
{

class Hud : public Screen
{
    sf::Font    _statusFont;

    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _zoneText;

    std::shared_ptr<sf::Text>           _healthText;
    std::shared_ptr<sf::Text>           _balanceText;

    PlayerPtr   _player;

public: 
    Hud(ResourceManager& resmgr, sf::RenderTarget& target)
        : Hud(resmgr, target, true)
    {}

    Hud(ResourceManager& resmgr, sf::RenderTarget& target, bool visible);

    void setZoneText(const std::string& zone);

    void setHealth(std::uint32_t health);
    void setBalance(float cash);
};

} // namespace