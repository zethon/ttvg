#include <sstream>

#include <boost/range/adaptor/indexed.hpp>

#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "../TTUtils.h"
#include "../Vehicle.h"
#include "../PathFactory.h"

#include "Opening.h"

namespace tt
{

constexpr auto SCENE_NAME = "tucson";

constexpr auto MAX_VEHICLES = 25u;
constexpr auto VEHICLE_SPAWN_RATE = 5u; // every X seconds
    
Opening::Opening(ResourceManager& resmgr, sf::RenderTarget& target, PlayerPtr player)
    : Scene{ SCENE_NAME, resmgr, target, player },
      _hud{ resmgr, target },
      _descriptionText{ resmgr, target },
      _debugWindow{ resmgr, target }
{
    sf::View view(sf::FloatRect(0.f, 0.f,
        static_cast<float>(_window.getSize().x), static_cast<float>(_window.getSize().y)));
    _window.setView(view);

    // the order in which we add everything to the draw'able
    // vector is important, so we do it all at the end of
    // the function
    // addDrawable(_background);

    initTraffic();

    _pgSoundBuffer = *(resmgr.load<sf::SoundBuffer>("sounds/playground.wav"));
    _pgSound.setBuffer(_pgSoundBuffer);
    _pgCenter = _background->getGlobalCenterFromTile(sf::Vector2f{ 140.f, 84.f });

    //_bgsong = _resources.openUniquePtr<sf::Music>("music/background_music1.wav");
    //_bgsong->setLoop(true);
    //_bgsong->play();
}

void Opening::enter()
{
    Scene::enter();

    // _player->setSource(0, 10);
    // _player->setOrigin(0.0f, 0.0f);

    _player->onSetHealth.connect(
        [this](std::uint32_t health)
        {
            _hud.setHealth(health);
        });

    _player->onSetCash.connect(
        [this](float cash)
        {
            _hud.setBalance(cash);
        });

    sf::Vector2f tile{ getPlayerTile() };
    auto tileinfo = _background->getTileInfo(tile);
    updateCurrentTile(tileinfo);

    _hud.setHealth(_player->health());
    _hud.setBalance(_player->balance());
}

void Opening::initTraffic()
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

PollResult Opening::poll(const sf::Event& e)
{
    auto retval = Scene::poll(e);
    if (retval.handled) return retval;

    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;

            //
            // Action. Perform an action. 
            // E.g. pick up an item or perform an action on an item.
            //
            case sf::Keyboard::A:
            {
                //
                // Check if player is standing on an item.
                //
                // Check if it is obtainable.
                //      If it is obtainable, remove it from the map's _items 
                //      and add it to the player's inventory.
                //
                // Check if it is actionable.
                //      If it is actionable, attempt to perform an action 
                //      on the item.
                //
                for(auto it = _items.begin(); it != _items.end(); it++)
                {
                    ItemPtr item = *it;

                    if( item->getGlobalBounds().intersects(
                                                _player->getGlobalBounds()) )
                    { 

                        if(item->isObtainable())
                        {
                            //
                            // Player obtains the item
                            //
                            _player->addItem(item);
                            _descriptionText.setText(
                                                "Picked up " + item->getName());
                            _items.erase(it);
                            break;
                        }

                        if(item->isActionable())
                        {

                            const auto& requiredItem = 
                                                item->getActionRequiresItem();

                            if(_player->hasItem(requiredItem))
                            {
                                //
                                // Player has the required item in their
                                // inventory to perform the action.
                                // It will cost them this item.
                                // This could also be optional. Not all
                                // actions should require an item.
                                //
                                _player->removeItem(requiredItem);
                                
                                //
                                // Remove the object we are
                                // operating on from the map.
                                // This should be optional, or change its
                                // state to reflect the action was performed.
                                // E.g. a closed chest might become open, but
                                // be empty. A locked door might become 
                                // unlocked or opened.
                                //
                                _items.erase(it);

                                //
                                // "Drop" the new item into place
                                // This should also be optional.
                                // Not all actions should result in a drop.
                                //
                                const auto& providedItem = 
                                                item->getActionProvidesItem();

                                auto itemFactory = ItemFactory(_resources);
                                sf::Vector2f position = item->getPosition();

                                ItemPtr p = itemFactory.createItem(
                                                            providedItem,
                                                            position);
                                _items.push_back(p);

                                //
                                // Show success message
                                //
                                _descriptionText.setText(
                                    item->getActionSuccessMsg());

                                break;

                            }
                            else
                            {

                                //
                                // Player does not have the required item
                                //
                                _descriptionText.setText(
                                    item->getActionFailureMsg());
                                break;

                            }
                        }
                    }
                }
            }
            break;

            //
            // Inventory. Display inventory.
            //
            case sf::Keyboard::I:
            {
                std::cout   << std::endl;

                const auto& inv = _player->getInventory();

                for (const auto& item : _player->getInventory())
                {
                    std::cout << item->getName() << '\n';
                }
                std::cout   << std::endl;
            }
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


            case sf::Keyboard::Num0:
            {
                _debugWindow.setVisible(!_debugWindow.visible());
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

ScreenAction Opening::timestep()
{
    timestepTraffic();

    Scene::timestep();

    std::stringstream ss;
    ss << _player->getGlobalCenter();
    std::stringstream ss1;
    ss1 << getPlayerTile();

    auto posText = fmt::format("P({},{})", ss.str(), ss1.str());
    _debugWindow.setText(posText);

    if (_player->health() <= 0)
    {
        //_bgsong->stop();
        return ScreenAction{ ScreenActionType::CHANGE_SCREEN, SCREEN_GAMEOVER };
    }

    return {};
}

void Opening::timestepTraffic()
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

void Opening::draw()
{
    // always adjust the view before drawing
    adjustView();

    // now to the base class drawing
    Scene::draw();

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
    _debugWindow.draw();
    _descriptionText.draw();
}

void Opening::adjustView()
{
    auto view = _window.getView();
    auto [xpos,ypos] = _player->getGlobalCenter();

    if (xpos < (_window.getSize().x / 2))
    {
        xpos = view.getCenter().x;
    }
    else if (auto totalWidth = _background->getGlobalBounds().width;
                xpos > (totalWidth - (_window.getSize().x / 2)))
    {
        xpos = totalWidth - (view.getSize().x / 2);
    }

    if (ypos < (_window.getSize().y / 2))
    {
        ypos = view.getCenter().y;
    }
    else if (auto totalHeight = _background->getGlobalBounds().height;
                ypos > (totalHeight - (_window.getSize().y / 2)))
    {
        ypos = totalHeight - (view.getSize().y / 2);
    }

    view.setCenter(xpos, ypos);
    _window.setView(view);
}

void Opening::updateCurrentTile(const TileInfo& info)
{
    _currentTile = info;

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
}

void Opening::toggleHighlight()
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
