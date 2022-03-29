#include <fmt/core.h>

#include "Hud.h"

namespace tt
{

Hud::Hud(ResourceManager & resmgr, sf::RenderTarget & target, GameCalendarPtr calendar, bool visible)
    : Screen(resmgr, target),
      _calendar{calendar}
{
    _background = std::make_shared<sf::RectangleShape>();
    _background->setFillColor(sf::Color{ 0, 0, 0, 175 });
    _background->setPosition(5.f, 2.f);
    _background->setSize(sf::Vector2f{ _window.getSize().x - 10.f, 60.f });
    addDrawable(_background);

    if (auto temp = _resources.load<sf::Font>("fonts/mono_bold.ttf");
            !temp.has_value())
    {
        throw std::runtime_error("could not load font mono_body.ttf");
    }
    else
    {
        _statusFont = * temp;
    }

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

    createCalendarWidgets();

    setVisible(visible);
}

void Hud::createCalendarWidgets()
{
    _dateText = std::make_unique<sf::Text>(_calendar->datestr(), _statusFont);
    _dateText->setCharacterSize(16);
    _dateText->setFillColor(sf::Color{228,254,254});

    _timeText = std::make_unique<sf::Text>(_calendar->timestr(), _statusFont);
    _timeText->setCharacterSize(16);
    _timeText->setFillColor(sf::Color{232,252,252});

    const auto boxwidth = _dateText->getGlobalBounds().width;
    const auto dateX = (_window.getSize().x - (boxwidth + 20.f));

    auto timeX = dateX + (_dateText->getGlobalBounds().width / 2);
    timeX -= _timeText->getGlobalBounds().width / 2;

    _dateText->setPosition(dateX, 2.5f);
    _timeText->setPosition(timeX, 21.5f);

    addDrawable(_dateText);
    addDrawable(_timeText);

    _calendar->onUpdate.connect(
            [this](const std::string& datestr, const std::string& timestr)
            {
                _dateText->setString(datestr);
                _timeText->setString(timestr);
            });
}

void Hud::setZoneText(const std::string & zone)
{
    if (!_zoneText) return;

    _zoneText->setString(zone);
    auto rect = _zoneText->getGlobalBounds();

    if (zone.size() > 0)
    {
        const auto xpos = (_window.getSize().x / 2) - (rect.width / 2);
        _zoneText->setPosition(xpos, 2.f);
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