#include "Hud.h"

namespace tt
{

Hud::Hud(ResourceManager & resmgr, sf::RenderTarget & target, bool visible)
    : Screen(resmgr, target)
{
    _background = std::make_shared<sf::RectangleShape>();
    _background->setFillColor(sf::Color{ 0, 0, 0, 175 });
    _background->setPosition(5.f, 2.f);
    _background->setSize(sf::Vector2f{ _window.getSize().x - 10.f, 40.f });
    addDrawable(_background);

    _statusFont = *(_resources.load<sf::Font>("fonts/mono_bold.ttf"));
    _zoneText = std::make_shared<sf::Text>("", _statusFont);
    _zoneText->setFillColor(sf::Color::White);
    _zoneText->setPosition(10.f, 2.5f);
    addDrawable(_zoneText);

    _balanceText = std::make_shared<sf::Text>("Cash: $40.00", _statusFont);
    _balanceText->setFillColor(sf::Color::White);
    _balanceText->setPosition(10.f, 2.5f);
    addDrawable(_balanceText);

    setVisible(visible);
}

void Hud::setZoneText(const std::string & zone)
{
    _zoneText->setString(zone);
    auto rect = _zoneText->getGlobalBounds();

    if (zone.size() > 0)
    {
        _zoneText->setPosition(_window.getSize().x - (rect.width + 10.f), 2.f);
    }
}

void Hud::setBalanceText(const std::string & text)
{
    _balanceText->setString(text);
}

} // namespace