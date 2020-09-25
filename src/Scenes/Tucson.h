#pragma once

#include <boost/range/adaptor/indexed.hpp>

#include <nlohmann/json.hpp>

#include <SFML/Graphics.hpp>

#include "../Vehicle.h"
#include "../VehicleFactory.h"

#include "../Background.h"
#include "../Player.h"

#include "Scene.h"

namespace nl = nlohmann;

namespace tt
{

class Tucson : public Scene
{

public:
    Tucson(const SceneSetup& setup);
    static constexpr auto SCENE_NAME = "Tucson";

    PollResult poll(const sf::Event& e) override;
    ScreenAction update(sf::Time elapsed);

private:
    void toggleHighlight() override;
    void customDraw() override;
    void customUpdateCurrentTile(const TileInfo&) override;

    void initTraffic();
    void timestepTraffic(sf::Time elapsed);

    nl::json                            _json;

    std::unique_ptr<VehicleFactory>     _vehicleFactory;
    std::vector<VehiclePtr>             _vehicles;
    bool                                _updateTraffic = true;

    //std::unique_ptr<sf::Music>          _bgsong;

    sf::SoundBuffer                     _pgSoundBuffer;
    sf::Sound                           _pgSound;
    sf::Vector2f                        _pgCenter;
    float                               _pgVolume = 0.f;

    bool    _showVehicleWarning = true;
};

} // namespace tt
