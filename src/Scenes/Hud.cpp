#include <fmt/core.h>

#include "Hud.h"

namespace tt
{

Hud::Hud(ResourceManager & resmgr, sf::RenderTarget & target, bool visible)
    : Screen(resmgr, target)
{
    _background = std::make_shared<sf::RectangleShape>();
    _background->setFillColor(sf::Color{ 0, 0, 0, 175 });
    _background->setPosition(5.f, 2.f);
    _background->setSize(sf::Vector2f{ _window.getSize().x - 10.f, 60.f });
    addDrawable(_background);

    if (auto temp = _resources.load<sf::Font>("fonts/mono_bold.ttf"); 
        temp.has_value())
    {
        _statusFont = *temp;
        _zoneText = std::make_shared<sf::Text>("", _statusFont);
        _zoneText->setFillColor(sf::Color::White);
        _zoneText->setPosition(10.f, 2.5f);
        addDrawable(_zoneText);

        _healthText = std::make_shared<sf::Text>(std::string{}, _statusFont);
        _healthText->setFillColor(sf::Color::White);
        _healthText->setPosition(10.f, 2.5f);
        _healthText->setCharacterSize(25);
        addDrawable(_healthText);

        _balanceText = std::make_shared<sf::Text>(std::string{}, _statusFont);
        _balanceText->setFillColor(sf::Color::White);
        _balanceText->setPosition(10.f, 30.0f);
        _balanceText->setCharacterSize(25);
        addDrawable(_balanceText);
    }
    else
    {
        // TODO: logging?!?!
    }

    setVisible(visible);
}

void Hud::setZoneText(const std::string & zone)
{
    if (!_zoneText) return;

    _zoneText->setString(zone);
    auto rect = _zoneText->getGlobalBounds();

    if (zone.size() > 0)
    {
        _zoneText->setPosition(_window.getSize().x - (rect.width + 10.f), 2.f);
    }
}

void Hud::setHealth(std::uint32_t health)
{
    if (!_healthText) return;

    std::string text = fmt::format("Health: {}", health);
    _healthText->setString(text);

    if (health < 40)
    {
        _healthText->setFillColor(sf::Color::Red);
    }
    else if (health < 70)
    {
        _healthText->setFillColor(sf::Color::Yellow);
    }
    else
    {
        _healthText->setFillColor(sf::Color::Green);
    }
}

void Hud::setBalance(float cash)
{
    if (!_balanceText) return;
    
    std::string text = fmt::format("Cash  : ${:.{}f}", cash, 2);
    _balanceText->setString(text);
}

} // namespace