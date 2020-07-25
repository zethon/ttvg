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

namespace nl = nlohmann;

namespace tt
{

class StatusBar : public Scene
{
    sf::Font    _statusFont;

    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _zoneText;
    std::shared_ptr<sf::Text>           _balanceText;

public: 
    StatusBar(ResourceManager& resmgr, sf::RenderTarget& target)
        : Scene(resmgr, target)
    {
        _background = std::make_shared<sf::RectangleShape>();
        _background->setFillColor(sf::Color{ 100, 100, 0, 104 });
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
    }

    void setZoneText(const std::string& zone)
    {
        _zoneText->setString(zone);
        auto rect = _zoneText->getGlobalBounds();

        if (zone.size() > 0)
        {
            _zoneText->setPosition(_window.getSize().x - (rect.width + 10.f), 2.f);
        }
    }

    void setBalanceText(const std::string& text)
    {
        _balanceText->setString(text);
    }
};

class DebugWindow : public Scene
{
    sf::Font    _debugFont;

    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _debugText;

public:
    DebugWindow(ResourceManager& resmgr, sf::RenderTarget& target)
        : Scene(resmgr, target)
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

class MissionText : public Scene
{
    sf::Font                            _font;
    std::shared_ptr<sf::Text>           _text;
    std::shared_ptr<sf::RectangleShape> _background;

public:
    MissionText(ResourceManager& resmgr, sf::RenderTarget& target)
        : Scene(resmgr, target)
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

class PathLines
{
    std::vector<sf::RectangleShape> _shapes;
    Background&                     _background;

public: 
    PathLines(Background& bg)
        : _background{ bg }
    {
        //sf::RectangleShape shape;
        //shape.setPosition(100.f, 8.f);
        //shape.setSize(sf::Vector2f(-100.f, 8.f));
        //shape.setFillColor(sf::Color::Yellow);
        //_shapes.push_back(shape);
    }
    
    void setPath(const Path& path)
    {
        _shapes.clear();
        const auto& points = path.points();
        if (points.size() == 0) return;

        auto prev = points.front();
        for (const auto& pt : path.points())
        {
            if (prev == pt) continue;

            // TODO: GET RID OF THESE TEMPS
            auto gPrev = _background.getGlobalFromTile(sf::Vector2f{ prev });
            auto gPt = _background.getGlobalFromTile(sf::Vector2f{ pt });

            float width = 0.f;
            float height = 0.f;
            sf::Vector2f topleft{ static_cast<float>(gPrev.x), static_cast<float>(gPrev.y) };

            if (prev.y == pt.y)
            {
                width = static_cast<float>(gPt.x - gPrev.x);
                height = 16.0f;
            }
            else
            {
                height = static_cast<float>(gPt.y - gPrev.y);
                width = 16.0f;
            }

            sf::RectangleShape shape;
            shape.setPosition(topleft);
            shape.setSize(sf::Vector2f{ width, height });

            _shapes.push_back(shape);
            prev = pt;
        }

        for (const auto& s : (_shapes | boost::adaptors::indexed()))
        {
            auto c = static_cast<sf::Uint8>((s.index() + 1) * (255 /_shapes.size()));
            s.value().setFillColor(sf::Color{ 0,c,c });
        }
    }

    void draw(sf::RenderTarget& window)
    {
        for (const auto& s : _shapes)
        {
            window.draw(s);
        }
    }

};

class Opening : public Scene
{

public:
    Opening(ResourceManager& resmgr, sf::RenderTarget& target);

    void createItems();

    std::uint16_t poll(const sf::Event& e) override;
    std::uint16_t timestep() override;
    void draw() override;

    sf::Vector2f getPlayerTile() const
    {
        auto playerxy = _player->getGlobalCenter();
        return _background->getTileFromGlobal(playerxy);
    }

private:

    void initTraffic();
    void timestepTraffic();

    void adjustView();
    void animeCallback();

    void updateMessage();

    MissionText                         _missionText;
    StatusBar                           _statusBar;
    DebugWindow                         _debugWindow;

    BackgroundSharedPtr                 _background;
    PlayerPtr                           _player;

    sf::Clock                           _globalClock;
    nl::json                            _json;

    std::unique_ptr<PathLines>          _pathLines;
    std::unique_ptr<VehicleFactory>     _vehicleFactory;
    std::vector<VehiclePtr>             _vehicles;

    std::unique_ptr<ItemFactory>        _itemFactory;
    std::vector<ItemPtr>                _items;

};

} // namespace tt
