#include <fmt/core.h>

#include "EuclidHouse.h"

namespace nl = nlohmann;

namespace tt
{
    
constexpr auto STEPSIZE = 16u;

EuclidHouse::EuclidHouse(ResourceManager& resmgr, sf::RenderTarget& target, PlayerPtr player)
    : Scene(resmgr, target, player),
      _hud{ resmgr, target },
      _debugWindow{ resmgr, target }
{
    _background = std::make_shared<Background>("EuclidHouse", _resources, sf::Vector2f{ 16, 16 });
    _background->setPosition(0.0f, 0.0f);

    const sf::Vector2f bgscale{
        static_cast<float>(_window.getSize().x) / static_cast<float>(_background->getTexture()->getSize().x),
        static_cast<float>(_window.getSize().y) / static_cast<float>(_background->getTexture()->getSize().y) };
    _background->setScale(bgscale);

    _hud.setVisible(false);

    addDrawable(_background);
}

ScreenAction EuclidHouse::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;

            case sf::Keyboard::Left:
            {
                if (_player->state() == AnimatedState::ANIMATED
                    && _player->direction() == Direction::LEFT)
                {
                    return Scene::poll(e);
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
                    return Scene::poll(e);
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
                    return Scene::poll(e);
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
                    return Scene::poll(e);
                }

                _player->setSource(0, 10);
                _player->setMaxFramesPerRow(9);
                _player->setState(AnimatedState::ANIMATED);
                _player->setDirection(Direction::DOWN);
            }
            break;

            case sf::Keyboard::Space:
            {
                if (_currentTile.type == TileType::ZONE)
                {
                    auto zone = boost::any_cast<Zone>(_currentTile.data);
                    if (zone.transition.has_value())
                    {
                        return { ScreenActionType::CHANGE_SCENE, zone.transition->newscene };
                    }
                }
            }
            break;

            case sf::Keyboard::Num0:
            {
                _debugWindow.setVisible(!_debugWindow.visible());
            }
            break;

            case sf::Keyboard::Period:
            {
                _hud.setVisible(!_hud.visible());
            }
            break;
        }
    }

    if (_player->state() == AnimatedState::ANIMATED
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
        && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        _player->setState(AnimatedState::STILL);
    }

    return Scene::poll(e);
}

ScreenAction EuclidHouse::timestep()
{
    std::stringstream ss;
    ss << _player->getGlobalCenter();
    std::stringstream ss1;
    ss1 << getPlayerTile();
    auto posText = fmt::format("P({},{})", ss.str(), ss1.str());
    _debugWindow.setText(posText);

    return Scene::timestep();
}

void EuclidHouse::draw()
{
    Scene::draw();
    _window.draw(*_player);

    _hud.draw();
    _debugWindow.draw();
}

void EuclidHouse::enter()
{
    Scene::enter();

    _player->setScale(2.25f, 2.25f);
    _player->setPosition(600.0f, 580.0f);
    _player->setAnimeCallback(
        [this]()->sf::Vector2f
        {
            if (walkPlayer(STEPSIZE))
            {
                sf::Vector2f tile{ getPlayerTile() };
                auto tileinfo = _background->getTileInfo(tile);
                updateCurrentTile(tileinfo);
            }
            return _player->getPosition();
        }
    );

    addUpdateable(_player);
}

void EuclidHouse::updateCurrentTile(const TileInfo & info)
{
    _currentTile = info;

    switch (_currentTile.type)
    {
        default:
            _hud.setZoneText({});
        break;

        case TileType::ZONE:
        {
            const auto zoneinfo = boost::any_cast<Zone>(_currentTile.data);
            _hud.setZoneText(zoneinfo.name);
        }
        break;
    }
}

} // namespace tt
