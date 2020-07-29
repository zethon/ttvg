#include <sstream>

#include <boost/range/adaptor/indexed.hpp>

#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "../TTUtils.h"
#include "../Item.h"
#include "../Vehicle.h"
#include "../PathFactory.h"

#include "Opening.h"

namespace tt
{

constexpr auto SCALE_PLAYER = 1.50f;
constexpr auto SCALE_BACKGROUND = 2.25f;

constexpr auto STEPSIZE = 16u;

constexpr auto PLAYER_START_X = 1616.0f;
constexpr auto PLAYER_START_Y = 2875.0f;

constexpr auto TILESIZE_X = 16.0f;
constexpr auto TILESIZE_Y = 16.0f;

constexpr auto MAPNAME = "tucson";

constexpr auto MAX_VEHICLES = 25u;
constexpr auto VEHICLE_SPAWN_RATE = 5u; // every X seconds
    
Opening::Opening(ResourceManager& resmgr, sf::RenderTarget& target)
    : Scene{ resmgr, target },
      _missionText { resmgr, target },
      _statusBar{ resmgr, target },
      _debugWindow{ resmgr, target }
{
    _resources.clearCaches();

    _background = std::make_shared<Background>(MAPNAME, _resources, sf::Vector2f { TILESIZE_X, TILESIZE_Y });
    _background->setScale(SCALE_BACKGROUND, SCALE_BACKGROUND);
    _background->setPosition(0.0f, 0.0f);

    sf::View view(sf::FloatRect(0.f, 0.f,
        static_cast<float>(_window.getSize().x), static_cast<float>(_window.getSize().y)));
    _window.setView(view);

    auto textptr = _resources.cacheTexture("textures/tommy.png");
    assert(textptr);
    textptr->setSmooth(true);

    _player = std::make_shared<Player>(*textptr, sf::Vector2i{ 64, 64 });
    _player->setSource(0, 10);
    _player->setScale(SCALE_PLAYER, SCALE_PLAYER);
    _player->setOrigin(0.0f, 0.0f);
    _player->setPosition(PLAYER_START_X, PLAYER_START_Y);
    _player->setAnimeCallback(
        [this]() 
        { 
            return this->animeCallback(); 
        });
        
    addUpdateable(_player);

    // the order in which we add everything to the draw'able
    // vector is important, so we do it all at the end of
    // the function
    addDrawable(_background);

    initTraffic();
    createItems();

    sf::Vector2f tile{ getPlayerTile() };
    _statusBar.setZoneText(_background->zoneName(tile));

    _missionText.setText("Find the magic vagina");
}

void Opening::createItems()
{
    //
    // Create items
    //
    _itemFactory = std::make_unique<ItemFactory>(_resources);

    //
    // This should probably come from a .json associated with map data.
    //

    ItemPtr sax = _itemFactory->createItem(
        "sax",
        sf::Vector2f{ 1516.0f, 2875.0f });
    _items.push_back(sax);

    ItemPtr menorah = _itemFactory->createItem(
        "menorah",
        sf::Vector2f{ 1416.0f, 2725.0f });
    _items.push_back(menorah);

    ItemPtr bag1 = _itemFactory->createItem(
        "bag-of-weed",
        sf::Vector2f{ 1216.0f, 3005.0f });
    _items.push_back(bag1);

    ItemPtr bag2 = _itemFactory->createItem(
        "bag-of-tobacco",
        sf::Vector2f{ 1206.0f, 3105.0f });
    _items.push_back(bag2);

    ItemPtr bag3 = _itemFactory->createItem(
                                    "bag-of-crack", 
        sf::Vector2f{ 1716.0f, 2975.0f });
    _items.push_back(bag3);
    ItemPtr bag4 = _itemFactory->createItem(
                                    "bag-of-crack",
                                    sf::Vector2f { 1756.0f, 2975.0f } );
    _items.push_back(bag4);

    ItemPtr bag5 = _itemFactory->createItem(
                                    "bag-of-crack",
                                    sf::Vector2f { 1796.0f, 2975.0f } );
    _items.push_back(bag5);

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

std::uint16_t Opening::poll(const sf::Event& e)
{

    if (e.type == sf::Event::KeyPressed)
    {
        updateMessage();

        switch (e.key.code)
        {
            default:
            break;

            //
            // Action. Perform an action. E.g. pick up an item.
            //
            case sf::Keyboard::A:
            {
                //
                // Check if we are on an item.
                // If yes, remove it from the map's _items and add it to the 
                // player's inventory.
                //
                for(auto it = _items.begin(); it != _items.end(); it++)
                {
                    ItemPtr item = *it;

                    if( item->getGlobalBounds().intersects(
                                                _player->getGlobalBounds()) &&
                        item->isObtainable() )
                    {

                        _player->addItem(item->getID());
                        _missionText.setText("Picked up " + item->getName());
                        _items.erase(it);

                        break;
                    }
                }
            }
            break;

            //
            // Inventory. Display inventory.
            //
            case sf::Keyboard::I:
            {
                const auto& inv = _player->getInventory();

                std::vector<std::string> keys;

                for(auto it = inv.begin(); it != inv.end(); it++)
                {
                    keys.push_back(it->first);
                }

                //
                // Sort keys, so we can display these in some kind of order.
                // Should really sort by display name rather than ID.
                //
                std::sort(keys.begin(), keys.end());

                for(auto it = keys.begin(); it != keys.end(); it++)
                {
                    std::string key = *it;
                    std::cout << key << ": " << inv.at(key) << std::endl;
                }
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

            case sf::Keyboard::Left:
            {
                if (_player->state() == AnimatedState::ANIMATED
                    && _player->direction() == Direction::LEFT)
                {
                    return 0;
                }

                _player->setSource(0, 9);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedState::ANIMATED);
                _player->setDirection(Direction::LEFT);
            }
            break;

            case sf::Keyboard::Right:
            {
                if (_player->state() == AnimatedState::ANIMATED
                    && _player->direction() == Direction::RIGHT)
                {
                    return 0;
                }

                _player->setSource(0, 11);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedState::ANIMATED);
                _player->setDirection(Direction::RIGHT);
            }
            break;

            case sf::Keyboard::Up:
            {
                if ((_player->state() == AnimatedState::ANIMATED && _player->direction() == Direction::UP)
                    || (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                {
                    return 0;
                }

                _player->setSource(0, 8);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedState::ANIMATED);
                _player->setDirection(Direction::UP);
            }
            break;

            case sf::Keyboard::Down:
            {
                if ((_player->state() == AnimatedState::ANIMATED && _player->direction() == Direction::DOWN)
                    || (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                {
                    return 0;
                }

                _player->setSource(0, 10);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedState::ANIMATED);
                _player->setDirection(Direction::DOWN);
            }
            break;

            case sf::Keyboard::Num0:
            {
                _debugWindow.setVisible(!_debugWindow.visible());
            }
            break;
        }
    }

    return 0;
}

std::uint16_t Opening::timestep()
{
    if (_player->state() == AnimatedState::ANIMATED
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        _player->setState(AnimatedState::STILL);
    }

    timestepTraffic();

    //
    // Check item bounds.
    //
    std::for_each(  _items.begin(), 
                    _items.end(),
                    [this](ItemPtr item) { 
                        if(item->getGlobalBounds().intersects(
                                                _player->getGlobalBounds())) {
                            _missionText.setText(
                                item->getName() + ": " +
                                item->getDescription() );
                        }
                    }
    );

    Scene::timestep();

    std::stringstream ss;
    ss << _player->getGlobalCenter();
    std::stringstream ss1;
    ss1 << getPlayerTile();
    auto posText = fmt::format("P({},{})", ss.str(), ss1.str());
    _debugWindow.setText(posText);

    return 0;
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
    std::for_each(  _items.begin(), 
                    _items.end(),
                    [this](ItemPtr item) { 
                        _window.draw(*item); 
                    }
    );

    // the player should always be the last thing on the 
    // game board to be drawn
    _window.draw(*_player);
  
    _window.setView(_window.getDefaultView());
    _statusBar.draw();
    _debugWindow.draw();
    _missionText.draw();
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

sf::Vector2f Opening::animeCallback()
{
    const auto stepSize = STEPSIZE 
        + (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ? 20 : 0);

    bool moved = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        auto xx = _player->getGlobalLeft();
        auto [x, y] = _player->getPosition();
        assert(xx >= 0);
        if (xx == 0) return _player->getPosition();

        xx -= stepSize;
        if (xx < 0) xx = 0;

        _player->setGlobalLeft(xx);
        moved = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        const auto boundaryRight = _background->getGlobalBounds().width;

        auto x = _player->getGlobalRight();
        assert(x <= boundaryRight);
        if (x == boundaryRight) return _player->getPosition();

        x += stepSize;
        if (x > boundaryRight) x = boundaryRight;
        _player->setGlobalRight(x);
        moved = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        auto y = _player->getGlobalTop();
        assert(y >= 0);
        if (y == 0) return _player->getPosition();

        y -= stepSize;
        if (y < 0) y = 0;
        _player->setGlobalTop(y);
        moved = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        const auto boundaryBottom = _background->getGlobalBounds().height;

        auto y = _player->getGlobalBottom();
        assert(y <= boundaryBottom);
        if (y == boundaryBottom) return _player->getPosition();

        y += stepSize;
        if (y > boundaryBottom)
        {
            y = boundaryBottom;
        }
        _player->setGlobalBottom(y);
        moved = true;
    }

    if (moved)
    {
        sf::Vector2f tile{ getPlayerTile() };
        _statusBar.setZoneText(_background->zoneName(tile));
    }

    return _player->getPosition();
}

//
// Only update message text with the generic mission message
// if the player moves. This allows the user to see  the result message 
// of any last action they may have performed.
//
void Opening::updateMessage()
{
    _missionText.setText("Find the magic vagina");
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
