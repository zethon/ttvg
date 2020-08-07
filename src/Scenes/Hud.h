#pragma once

#include "../Screen.h"

namespace tt
{

class Hud : public Screen
{
    sf::Font    _statusFont;

    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _zoneText;
    std::shared_ptr<sf::Text>           _balanceText;

public: 
    Hud(ResourceManager& resmgr, sf::RenderTarget& target)
        : Hud(resmgr, target, true)
    {}

    Hud(ResourceManager& resmgr, sf::RenderTarget& target, bool visible);

    void setZoneText(const std::string& zone);
    void setBalanceText(const std::string& text);
};

} // namespace