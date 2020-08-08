#pragma once

#include <boost/range/adaptor/indexed.hpp>

#include <nlohmann/json.hpp>

#include <SFML/Graphics.hpp>

#include "../Vehicle.h"
#include "../VehicleFactory.h"

#include "../Item.h"
#include "../ItemFactory.h"

#include "../Background.h"
#include "../Player.h"

#include "Scene.h"
#include "Hud.h"

namespace nl = nlohmann;

namespace tt
{

class DebugWindow : public Screen
{
    sf::Font    _debugFont;

    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _debugText;

public:
    DebugWindow(ResourceManager& resmgr, sf::RenderTarget& target)
        : Screen(resmgr, target)
    {
        setVisible(false);

        if (auto temp = _resources.load<sf::Font>("fonts/mono_bold.ttf");
                !temp.has_value())
        {
            throw std::runtime_error("could not load resource 'fonts/mono_bold.ttf'");
        }
        else
        {
            _debugFont = *(temp);
        }

        _background = std::make_shared<sf::RectangleShape>();
        _background->setFillColor(sf::Color{ 0, 0, 0, 104 });
        _background->setPosition(0.f, _window.getSize().y - 40.f);
        _background->setSize(sf::Vector2f{ static_cast<float>(_window.getSize().x), 40.f });
        addDrawable(_background);

        _debugText = std::make_shared<sf::Text>("", _debugFont);
        _debugText->setFillColor(sf::Color::White);
        _debugText->setPosition(10.f, _window.getSize().y - 35.f);
        _debugText->setCharacterSize(20);
        addDrawable(_debugText);
    }

    void setText(const std::string& text)
    {
        _debugText->setString(text);
    }
};

class MissionText : public Screen
{
    sf::Font                            _font;
    std::shared_ptr<sf::Text>           _text;
    std::shared_ptr<sf::RectangleShape> _background;

public:
    MissionText(ResourceManager& resmgr, sf::RenderTarget& target)
        : Screen(resmgr, target)
    {
        _background = std::make_shared<sf::RectangleShape>();
        _background->setFillColor(sf::Color{ 0, 0, 0, 175 });
        addDrawable(_background);

        _font = *(_resources.load<sf::Font>("fonts/mono_bold.ttf"));
        _text = std::make_shared<sf::Text>("", _font);
        _text->setFillColor(sf::Color::Yellow);
        _text->setCharacterSize(25);
        addDrawable(_text);
    }

    void setText(const std::string& text)
    {
        if (text.size() == 0)
        {
            setVisible(false);
            return;
        }

        setVisible(true);
        _text->setString(text);

        auto rect = _text->getGlobalBounds();
        _text->setPosition((_window.getSize().x /2) - (rect.width / 2), 42.5f);
        
        auto[x, y, w, h] = _text->getGlobalBounds();

        _background->setSize(sf::Vector2f{ w + 20.f, h + 10.f });
        _background->setPosition(x - 10.f, y - 5.f);
    }

};

class Opening : public Scene
{

public:
    Opening(ResourceManager& resmgr, sf::RenderTarget& target, PlayerPtr player);

    void createItems();

    ScreenAction poll(const sf::Event& e) override;
    ScreenAction timestep() override;
    void draw() override;

    void enter() override;
    void exit() override;

private:

    void initTraffic();
    void timestepTraffic();

    void adjustView();
    sf::Vector2f animeCallback();

    void updateCurrentTile(const TileInfo& info) override;

    void toggleHighlight();

    MissionText                         _missionText;
    Hud                                 _hud;
    DebugWindow                         _debugWindow;

    sf::Clock                           _globalClock;
    nl::json                            _json;

    std::unique_ptr<VehicleFactory>     _vehicleFactory;
    std::vector<VehiclePtr>             _vehicles;
    bool                                _updateTraffic = true;

    std::unique_ptr<ItemFactory>        _itemFactory;
    std::vector<ItemPtr>                _items;
};

} // namespace tt
