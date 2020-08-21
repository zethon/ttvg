#pragma once

#include <boost/range/adaptor/indexed.hpp>

#include <nlohmann/json.hpp>

#include <SFML/Graphics.hpp>

#include "../Vehicle.h"
#include "../VehicleFactory.h"

#include "../Background.h"
#include "../Player.h"

#include "Scene.h"
#include "Hud.h"
#include "DescriptionText.h"

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

class Opening : public Scene
{

public:
    Opening(ResourceManager& resmgr, sf::RenderTarget& target, PlayerPtr player);

    PollResult poll(const sf::Event& e) override;
    ScreenAction timestep() override;
    void draw() override;

    void enter() override;

protected:
    void updateCurrentTile(const TileInfo& info) override;

private:
    void initTraffic();
    void timestepTraffic();
    void adjustView();
    void toggleHighlight();

    Hud                                 _hud;
    DescriptionText                     _descriptionText;
    DebugWindow                         _debugWindow;

    sf::Clock                           _globalClock;
    nl::json                            _json;

    std::unique_ptr<VehicleFactory>     _vehicleFactory;
    std::vector<VehiclePtr>             _vehicles;
    bool                                _updateTraffic = true;

    //std::unique_ptr<sf::Music>          _bgsong;

    sf::SoundBuffer                     _pgSoundBuffer;
    sf::Sound                           _pgSound;
    sf::Vector2f                        _pgCenter;
    float                               _pgVolume = 0.f;
};

} // namespace tt
