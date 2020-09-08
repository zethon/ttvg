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
    : Scene{ SCENE_NAME, setup }
{
     initTraffic();

    //_pgSoundBuffer = *(resmgr.load<sf::SoundBuffer>("sounds/playground.wav"));
    //_pgSound.setBuffer(_pgSoundBuffer);
    //_pgCenter = _background->getGlobalCenterFromTile(sf::Vector2f{ 140.f, 84.f });

    //_bgsong = _resources.openUniquePtr<sf::Music>("music/background_music1.wav");
    //_bgsong->setLoop(true);
    //_bgsong->play();
}

void Tucson::enter()
{
    Scene::enter();

    sf::Vector2f tile{ getPlayerTile() };
    auto tileinfo = _background->getTileInfo(tile);
    updateCurrentTile(tileinfo);

    _hud.setHealth(_player->health());
    _hud.setBalance(_player->balance());
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

            case sf::Keyboard::H:
            {
                toggleHighlight();
            }
            break;

            case sf::Keyboard::P:
            {
                _updateTraffic = !_updateTraffic;
            }
            break;


            case sf::Keyboard::LBracket:
            {
                _player->reduceHealth(10);
            }
            break;

            case sf::Keyboard::RBracket:
            {
                _player->increaseHealth(10);
            }
            break;
        }
    }

    return retval;
}

ScreenAction Tucson::timestep()
{
    timestepTraffic();

    Scene::timestep();

    if (_player->health() <= 0)
    {
        //_bgsong->stop();
        return ScreenAction{ ScreenActionType::CHANGE_SCREEN, SCREEN_GAMEOVER };
    }

    return {};
}

void Tucson::timestepTraffic()
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
        if (result == Vehicle::DELETE)
        {
            // remove it from our vehicle list
            vi = _vehicles.erase(vi);
            continue;
        }
        else if (ptr->isBlocked(playerBounds))
        {
            ptr->setVehicleState(Vehicle::STOPPED);
        }
        else if (ptr->vehicleState() == Vehicle::STOPPED)
        {
            ptr->setVehicleState(Vehicle::MOVING);
        }

        vi++;
    }

    auto runSeconds = static_cast<std::uint32_t>(_globalClock.getElapsedTime().asSeconds());
    if (_vehicles.size() < MAX_VEHICLES
        && ((runSeconds % VEHICLE_SPAWN_RATE) == 0) && runSeconds != 0)
    {
        auto vehicle = _vehicleFactory->createVehicle();
        _vehicles.push_back(vehicle);
    }
}

void Tucson::draw()
{
    // always adjust the view before drawing
    adjustView();

    // skip `Scene::draw` and instead call `Screen::draw` in
    // order to draw the background
    Screen::draw();

    // draw the vehicles
    std::for_each(_vehicles.begin(), _vehicles.end(),
        [this](VehiclePtr v) { _window.draw(*v); });

    //
    // Draw items
    //
    std::for_each(_items.begin(), _items.end(),
        [this](ItemPtr item) 
        { 
            _window.draw(*item); 
        });

    // the player should always be the last thing on the 
    // game board to be drawn
    _window.draw(*_player);
  
    _window.setView(_window.getDefaultView());
    _hud.draw();
    _descriptionText.draw();
    _debugWindow.draw();
}

void Tucson::updateCurrentTile(const TileInfo& info)
{
    _currentTile = info;

    //// TODO: PUT THIS IN A CONFIG FILE
    //// BEGIN MESSY HARCODED PLAYGROUND SOUND CODE TO BE REMOVED
    //auto pgdist = tt::distance(_pgCenter, _player->getGlobalCenter());
    //if (pgdist < 400.0f)
    //{
    //    if (_pgSound.getStatus() != sf::SoundSource::Status::Playing)
    //    {
    //        _pgSound.setVolume(100.0);
    //        _pgSound.play();
    //    }
    //}
    //else if (pgdist < 900.0f)
    //{
    //    float volume = (1.f - ((pgdist - 400.f) / 500.f)) * 100.f;
    //    _pgSound.setVolume(volume);
    //    if (_pgSound.getStatus() != sf::SoundSource::Status::Playing)
    //    {
    //        _pgSound.play();
    //    }
    //}
    //else
    //{
    //    _pgSound.setVolume(0.f);
    //    _pgSound.pause();
    //}
    //// END UGLY HARDCOED PLAYGROUND CODE

    bool handled = false;
    std::for_each(_items.begin(), _items.end(),
        [this, &handled](ItemPtr item) 
        {
            if (item->getGlobalBounds().intersects(_player->getGlobalBounds())) 
            {
                _descriptionText.setText(
                    item->getName() + ": " +
                    item->getDescription());

                handled = true;
            }
        }
    );

    if (handled) return;

    switch (_currentTile.type)
    {
        default:
            _hud.setZoneText({});
            _descriptionText.setText({});
        break;

        case TileType::ZONE:
        {
            const auto zoneinfo = boost::any_cast<Zone>(_currentTile.data);
            _hud.setZoneText(zoneinfo.name);
            if (!zoneinfo.description.empty())
            {
                _descriptionText.setText(zoneinfo.description);
            }
        }
        break;
    }

    std::stringstream ss;
    ss << _player->getGlobalCenter();
    std::stringstream ss1;
    ss1 << getPlayerTile();

    auto posText = fmt::format("P({},{})", ss.str(), ss1.str());
    _debugWindow.setText(posText);
}

void Tucson::toggleHighlight()
{
    _player->setHighlighted(!_player->highlighted());
    _vehicleFactory->setHighlighted(!_vehicleFactory->highlighted());

    for (auto& v : _vehicles)
    {
        v->setHighlighted(!v->highlighted());
    }

    for (auto& i : _items)
    {
        i->setHighlighted(!i->highlighted());
    }
}

} // namespace tt
