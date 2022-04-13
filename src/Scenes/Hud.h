#pragma once

#include "../Screens/Screen.h"

#include "../Player.h"
#include "../GameWorld.h"

namespace tt
{

class Hud : public Screen
{
public: 
    Hud(ResourceManager& resmgr, sf::RenderTarget& target, GameCalendarPtr calendar)
        : Hud(resmgr, target, calendar, true)
    {
        // nothing to do
    }

    Hud(ResourceManager& resmgr, sf::RenderTarget& target, GameCalendarPtr calendar, bool visible);

    void setZoneText(const std::string& zone);

    void setHealth(std::uint32_t health);
    void setBalance(float cash);

private:
    void createCalendarWidgets();

    sf::Font    _statusFont;

    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _zoneText;

    std::shared_ptr<sf::Text>           _healthText;
    std::shared_ptr<sf::Text>           _balanceText;

    std::shared_ptr<sf::Text>           _dateText;
    std::shared_ptr<sf::Text>           _timeText;

    GameCalendarPtr                     _calendar;
};

} // namespace