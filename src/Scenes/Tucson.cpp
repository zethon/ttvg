#include <sstream>

#include <boost/range/adaptor/indexed.hpp>

#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "../TTUtils.h"
#include "../Vehicle.h"
#include "../PathFactory.h"

#include "Tucson.h"

namespace tt
{

constexpr auto MAX_VEHICLES = 25u;
constexpr auto VEHICLE_SPAWN_RATE = 5u; // every X seconds
    
Tucson::Tucson(const SceneSetup& setup)
    : Scene{ SCENE_NAME, setup },
      _hackerTerminal{setup.resources, setup.window}
{
     initTraffic();

    _pgSoundBuffer = *(_resources.load<sf::SoundBuffer>("sounds/playground.wav"));
    _pgSound.setBuffer(_pgSoundBuffer);
    _pgCenter = _background->getGlobalCenterFromTile(sf::Vector2f{ 140.f, 84.f });

    _hackerTerminal.setVisible(false);
}

void Tucson::initTraffic()
{
    // initialize the traffic system
    _vehicleFactory = std::make_unique<VehicleFactory>(_resources, _background);

    auto[x, y, widthf, heightf] = _background->getWorldTileRect();
    assert(widthf > 0);
    assert(heightf > 0);

    sf::Vector2i size{ static_cast<int>(widthf), static_cast<int>(heightf) };
   auto pathFactory = std::make_shared<PathFactory>(size);

    TurningPointParser eparser;
    std::vector<TurningPoint> edges;
    const auto& config = _background->json();
    for (const auto& item : config["vehicles"]["edges"].items())
    {
        std::string value{ item.value().get<std::string>() };
        auto edge = eparser.parse(value.begin(), value.end());
        if (edge.has_value()) edges.push_back(*edge);
    }
    pathFactory->setEdges(edges);

    IntersectionParser iparser;
    std::vector<TurningPoint> intersections;
    for (const auto& item : config["vehicles"]["intersections"].items())
    {
        std::string value{ item.value().get<std::string>() };
        auto result = iparser.parse(value.begin(), value.end());
        if (result.has_value())
        {
            auto[origin, type, hzdbl, vtdbl] = *result;
            auto tempv = tt::makeIntersection(sf::Vector2i{ origin }, type, hzdbl, vtdbl);
            intersections.insert(intersections.end(), tempv.begin(), tempv.end());
        }
    }
    pathFactory->setTurningPoints(intersections);

    edges.clear();
    for (const auto& item : config["vehicles"]["turningpoints"].items())
    {
        std::string value{ item.value().get<std::string>() };
        auto tp = eparser.parse(value.begin(), value.end());
        if (tp.has_value())
        {
            pathFactory->addTurn(*tp);
        }
    }
    _vehicleFactory->setPathFactory(pathFactory);
}

PollResult Tucson::poll(const sf::Event& e)
{
    auto retval = Scene::poll(e);
    if (retval.handled) return retval;

    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;

            case sf::Keyboard::P:
            {
                _updateTraffic = !_updateTraffic;
            }
            break;

            case sf::Keyboard::F1:
            {
                MessagesWindow w{ *this };
                w.pushMessage(R"(
Welcome to the Tommy Tucson Video Game Tutorial!!

Or press SPACE to continue
Press ESC to skip tutorial. )");

                w.pushMessage("Use the arrow keys to walk around");
                w.pushMessage("You can enter new areas and explore by\npressing the SPACE bar");
                w.pushMessage("If you find something interesting you can\npick it up by pressing A");
                w.pushMessage("You can display your inventory by pressing I");
                w.pushMessage("And if you're a hobo and want to quit, press ESCAPE!");
                w.pushMessage("That's it! Namaste motherfucker!");
                w.exec();
            }
            break;

#ifndef _WINDOWS
            case sf::Keyboard::Slash:
            {
                _hackerTerminal.setVisible(!_hackerTerminal.visible());
            }
            break;
#endif
        }
    }

    return retval;
}

ScreenAction Tucson::update(sf::Time elapsed)
{
    timestepTraffic(elapsed);
    return Scene::update(elapsed);
}

void Tucson::timestepTraffic(sf::Time elapsed)
{
    if (!_updateTraffic) return;

    // update existing vehicles before creating any new ones
    // this also gives newly created vehicles a chance to be
    // drawn at their initial position
    static sf::Clock test;
    auto vi = _vehicles.begin();
    const auto playerBounds = _player->getGlobalBounds();
    while (vi != _vehicles.end())
    {
        auto& ptr = *vi;
        auto result = ptr->timestep();
        if (result == Vehicle::DELETE_VEHICLE)
        {
            // remove it from our vehicle list
            vi = _vehicles.erase(vi);
            continue;
        }
        else if (ptr->isBlocked(playerBounds))
        {
            if (ptr->vehicleState() == Vehicle::MOVING)
            {
                _player->reduceHealth(ptr->damage());
                ptr->playHornSound();

                if (_showVehicleWarning)
                {
                    _showVehicleWarning = false;
                    ModalWindow w{ *this };
                    w.setText("Be careful! Cars can cause damage and can kill you!\nLook both ways before crossing the street\n\nIf your health is too low then visit a hospital\n(but it ain't free)");
                    w.exec();
                }
                else
                {
                    _descriptionText.setText("Ouch!!!!");
                }
            }

            ptr->setVehicleState(Vehicle::STOPPED);
        }
        else if (ptr->vehicleState() == Vehicle::STOPPED)
        {
            ptr->setVehicleState(Vehicle::MOVING);
        }

        vi++;
    }

    auto runSeconds = static_cast<std::uint32_t>(elapsed.asSeconds());
    if (_vehicles.size() < MAX_VEHICLES
        && ((runSeconds % VEHICLE_SPAWN_RATE) == 0) && runSeconds != 0)
    {
        auto vehicle = _vehicleFactory->createVehicle();
        _vehicles.push_back(vehicle);
    }
}

void Tucson::beforeDraw()
{
    // draw the vehicles
    std::for_each(_vehicles.begin(), _vehicles.end(),
        [this](VehiclePtr v) 
        { 
            _window.draw(*v); 
        });
}

void Tucson::afterDraw()
{
    _hackerTerminal.draw();
}

void Tucson::customUpdateCurrentTile(const TileInfo& info)
{
    // TODO: PUT THIS IN A CONFIG FILE
    // BEGIN MESSY HARCODED PLAYGROUND SOUND CODE TO BE REMOVED
    auto pgdist = tt::distance(_pgCenter, _player->getGlobalCenter());
    if (pgdist < 400.0f)
    {
        if (_pgSound.getStatus() != sf::SoundSource::Status::Playing)
        {
            _pgSound.setVolume(100.0);
            _pgSound.play();
        }
    }
    else if (pgdist < 900.0f)
    {
        float volume = (1.f - ((pgdist - 400.f) / 500.f)) * 100.f;
        _pgSound.setVolume(volume);
        if (_pgSound.getStatus() != sf::SoundSource::Status::Playing)
        {
            _pgSound.play();
        }
    }
    else
    {
        _pgSound.setVolume(0.f);
        _pgSound.pause();
    }
    // END UGLY HARDCOED PLAYGROUND CODE
}

void Tucson::toggleHighlight()
{
    Scene::toggleHighlight();
    _vehicleFactory->setHighlighted(!_vehicleFactory->highlighted());

    for (auto& v : _vehicles)
    {
        v->setHighlighted(!v->highlighted());
    }
}

} // namespace tt
