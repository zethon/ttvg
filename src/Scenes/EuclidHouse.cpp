#include "EuclidHouse.h"

namespace nl = nlohmann;

namespace tt
{
    
EuclidHouse::EuclidHouse(ResourceManager& resmgr, sf::RenderTarget& target, PlayerPtr player)
    : Scene(resmgr, target, player)
{
    _background = std::make_shared<Background>("EuclidHouse", _resources, sf::Vector2f{ 64, 64 });
    _background->setPosition(0.0f, 0.0f);

    const sf::Vector2f bgscale{
        static_cast<float>(_window.getSize().x) / static_cast<float>(_background->getTexture()->getSize().x),
        static_cast<float>(_window.getSize().y) / static_cast<float>(_background->getTexture()->getSize().y) };
    _background->setScale(bgscale);

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
                return { ScreenActionType::CHANGE_SCENE, 0 };
            }
        }
    }

    return Scene::poll(e);
}

void EuclidHouse::draw()
{
    Scene::draw();
    _window.draw(*_player);
}

void EuclidHouse::enter()
{
    Scene::enter();

    //_player->setAnimeCallback(
    //    [this]()->sf::Vector2f
    //    {

    //    }
    //);
}

} // namespace tt
