#pragma once

#include <boost/range/adaptor/indexed.hpp>

#include <nlohmann/json.hpp>

#include <SFML/Graphics.hpp>

#include "../Vehicle.h"
#include "../VehicleFactory.h"

#include "../Background.h"
#include "../Player.h"

#include "Scene.h"
#include "HackerTerminal.h"

namespace nl = nlohmann;

namespace tt
{

class Tucson : public Scene
{

public:
    Tucson(const SceneSetup& setup);
    static constexpr auto SCENE_NAME = "Tucson";

    PollResult poll(const sf::Event& e) override;
    ScreenAction update(sf::Time elapsed) override;

private:
    void toggleHighlight() override;
    void customUpdateCurrentTile(const TileInfo&) override;

    void beforeDraw() override;
    void afterDraw() override;

    void initTraffic();
    void timestepTraffic(sf::Time elapsed);

    nl::json                            _json;

    std::unique_ptr<VehicleFactory>     _vehicleFactory;
    std::vector<VehiclePtr>             _vehicles;
    bool                                _updateTraffic = true;
    bool                                _vehicleCollision = true;

    sf::SoundBuffer                     _pgSoundBuffer;
    sf::Sound                           _pgSound;
    sf::Vector2f                        _pgCenter;
    float                               _pgVolume = 0.f;

    HackerTerminal                      _hackerTerminal;

    bool    _showVehicleWarning = true;
};

} // namespace tt
