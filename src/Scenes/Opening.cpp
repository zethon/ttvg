#include <sstream>

#include <boost/range/adaptor/indexed.hpp>

#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "../TTUtils.h"
#include "../Vehicle.h"

#include "Opening.h"

namespace tt
{

constexpr auto SCALE_PLAYER = 1.50f;
constexpr auto SCALE_BACKGROUND = 2.25f;

constexpr auto STEPSIZE = 16u;

constexpr auto PLAYER_START_X = 1616.0f;
constexpr auto PLAYER_START_Y = 2875.0f;

constexpr auto TILESIZE_X = 10;
constexpr auto TILESIZE_Y = 10;
    
Opening::Opening(ResourceManager& resmgr, sf::RenderTarget& target)
    : Scene{ resmgr, target },
      _statusBar{ resmgr, target },
      _debugWindow{ resmgr, target }
{
    {
        const std::string jsonfile =
            resmgr.getFilename("maps/tucson.json");

        std::ifstream file(jsonfile.c_str());
        file >> _json;
    }

    _background = std::make_shared<Background>("tucson", _resources, sf::Vector2i { TILESIZE_X, TILESIZE_Y });
    _background->setScale(SCALE_BACKGROUND, SCALE_BACKGROUND);
    _background->setPosition(0.0f, 0.0f);

    // auto top = (_background->texture().getSize().y * 0.7f) - _window.getSize().y;
    sf::View view(sf::FloatRect(0.f, 0.f,
        static_cast<float>(_window.getSize().x), static_cast<float>(_window.getSize().y)));
    _window.setView(view);

    auto temptext = *(_resources.load<sf::Texture>("textures/tommy.png"));
    _player = std::make_shared<Player>(temptext, sf::Vector2i{ 64, 64 });
    _player->texture().setSmooth(true);
    _player->setSource(0, 2);
    _player->setScale(SCALE_PLAYER, SCALE_PLAYER);
    _player->setOrigin(0.0f, 0.0f);
    _player->setPosition(PLAYER_START_X, PLAYER_START_Y);
    _player->setAnimeCallback([this]() { this->animeCallback(); });
    addUpdateable(_player);

    // the order in which we add everything to the draw'able
    // vector is important, so we do it all at the end of
    // the function
    addDrawable(_background);

    sf::Vector2f tile{ getPlayerTile() };
    _statusBar.setZoneText(_background->zoneName(tile));
}

std::uint16_t Opening::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;

            case sf::Keyboard::Left:
            {
                if (_player->state() == AnimatedSprite::ANIMATED
                    && _player->direction() == AnimatedSprite::LEFT)
                {
                    return 0;
                }

                _player->setSource(0, 9);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedSprite::ANIMATED);
                _player->setDirection(AnimatedSprite::LEFT);
            }
            break;

            case sf::Keyboard::Right:
            {
                if (_player->state() == AnimatedSprite::ANIMATED
                    && _player->direction() == AnimatedSprite::RIGHT)
                {
                    return 0;
                }

                _player->setSource(0, 11);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedSprite::ANIMATED);
                _player->setDirection(AnimatedSprite::RIGHT);
            }
            break;

            case sf::Keyboard::Up:
            {
                if ((_player->state() == AnimatedSprite::ANIMATED && _player->direction() == AnimatedSprite::UP)
                    || (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                {
                    return 0;
                }

                _player->setSource(0, 8);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedSprite::ANIMATED);
                _player->setDirection(AnimatedSprite::UP);
            }
            break;

            case sf::Keyboard::Down:
            {
                if ((_player->state() == AnimatedSprite::ANIMATED && _player->direction() == AnimatedSprite::DOWN)
                    || (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                {
                    return 0;
                }

                _player->setSource(0, 10);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedSprite::ANIMATED);
                _player->setDirection(AnimatedSprite::DOWN);
            }
            break;

            case sf::Keyboard::Num0:
            {
                _debugWindow.setVisible(!_debugWindow.visible());
            }
            break;

            case sf::Keyboard::A: // LEFT
            {
                sf::View view = _window.getView();
                auto [x,y] = view.getCenter();
                if ((x - (view.getSize().x / 2)) > 0)
                {
                    x -= 20;
                    view.setCenter(x, y);
                    _window.setView(view);
                }
            }
            break;

            case sf::Keyboard::D: // RIGHT
            {
                sf::View view = _window.getView();
                auto [x,y] = view.getCenter();
                if (((x+20) + (view.getSize().x / 2))
                    < _background->getRightBoundary())
                {
                    x += 20;
                    view.setCenter(x, y);
                    _window.setView(view);
                }
            }
            break;

            case sf::Keyboard::W: // UP
            {
                sf::View view = _window.getView();
                auto [x,y] = view.getCenter();
                if ((y - (view.getSize().y / 2)) > 20)
                {
                    y -= 20;
                    view.setCenter(x, y);
                    _window.setView(view);
                }
            }
            break;

            case sf::Keyboard::S: // DOWN
            {
                sf::View view = _window.getView();
                auto [x,y] = view.getCenter();

                if ((y + (view.getSize().y / 2)) 
                    < _background->getBottomBoundary())
                {
                    y += 20;
                    view.setCenter(x, y);
                    _window.setView(view);
                }
            }
            break;

        }
    }

    return 0;
}

std::uint16_t Opening::timestep()
{
    if (_player->state() == AnimatedSprite::ANIMATED
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        _player->setState(AnimatedSprite::STILL);
    }

    if (_globalClock.getElapsedTime().asSeconds() > 2)
    {
        spawnNPC();
        _testSpawned = true;
    }

    static sf::Clock test;
    auto vi = _vehicles.begin();
    while (vi != _vehicles.end())
    {
        auto& ptr = *vi;
        auto result = ptr->timestep();
        if (result == Vehicle::DELETE)
        {
            auto oi = std::find(_objects.begin(), _objects.end(), ptr);
            if (oi != _objects.end())
            {
                // let `Screen::draw()` remove the object from the 
                // container
                oi->reset();
            }

            // remove it from our vehicle list
            vi = _vehicles.erase(vi);
            continue;
        }

        if (ptr->isBlocked(_player->getGlobalBounds()))
        {
            ptr->setVehicleState(Vehicle::STOPPED);
        }
        else if (ptr->vehicleState() == Vehicle::STOPPED)
        {
            ptr->setVehicleState(Vehicle::MOVING);
        }

        vi++;
    }

    Scene::timestep();

    auto[cx, cy] = _player->getGlobalCenter();
    auto[tilex, tiley] = getPlayerTile();

    std::stringstream ss;
    if (_vehicles.size() > 0)
    {
        auto carbounds = _vehicles.front()->getGlobalBounds();
        ss << carbounds;
    }

    std::stringstream ss1;
    ss1 << _player->getGlobalBounds();

//    auto posText = fmt::format("XY:({},{}) LOC:({},{}) CAR:{}\n", 
//        cx, cy, tilex, tiley, cartext);

    auto posText = fmt::format("P({}) C({})", ss1.str(), ss.str());

    _debugWindow.setText(posText);

    return 0;
}

sf::Vector2f Opening::getPlayerTile() const
{
    auto pc = _player->getGlobalCenter();
    return tt::getTileFromGlobal(_player->getPosition(), { TILESIZE_X, TILESIZE_Y });
}

void Opening::draw()
{
    adjustView();
    Scene::draw();

    // the player should always be the last thing on the 
    // game board to be drawn
    _window.draw(*_player);
    
    _window.setView(_window.getDefaultView());
    _statusBar.draw();
    _debugWindow.draw();
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

void Opening::animeCallback()
{
    const auto stepSize = STEPSIZE 
        + (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ? 20 : 0);

    bool moved = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        auto xx = _player->getGlobalLeft();
        const auto boundaryLeft = _background->getLeftBoundary();
        auto [x, y] = _player->getPosition();
        assert(xx >= boundaryLeft);
        if (xx == boundaryLeft) return;

        xx -= stepSize;
        if (xx < boundaryLeft) xx = boundaryLeft;

        _player->setGlobalLeft(xx);
        moved = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        const auto boundaryRight = _background->getRightBoundary();

        auto x = _player->getGlobalRight();
        assert(x <= boundaryRight);
        if (x == boundaryRight) return;

        x += stepSize;
        if (x > boundaryRight) x = boundaryRight;
        _player->setGlobalRight(x);
        moved = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        const auto boundaryTop = _background->getTopBoundary();
        auto y = _player->getGlobalTop();
        assert(y >= boundaryTop);
        if (y == boundaryTop) return;

        y -= stepSize;
        if (y < boundaryTop) y = boundaryTop;
        _player->setGlobalTop(y);
        moved = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        const auto boundaryBottom = _background->getBottomBoundary();

        auto y = _player->getGlobalBottom();
        assert(y <= boundaryBottom);
        if (y == boundaryBottom) return;

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
}

void Opening::spawnNPC()
{
    if (_testSpawned) return;

    auto temptext = *(_resources.load<sf::Texture>("textures/car1.png"));
    auto npc = std::make_shared<Vehicle>(temptext, sf::Vector2i{ 77, 41 }, _background);
    // npc->setBackground(_background);
    
    auto [x,y] = _player->getGlobalCenter();
    // npc->setPosition(x, y + 20.0f);
    // _player->setAnimeCallback([this]() { this->animeCallback(); });
    // addUpdateable(npc);
    _vehicles.push_back(npc);
    addDrawable(npc);
}

} // namespace tt