#include <boost/range/adaptor/indexed.hpp>

#include <fmt/core.h>

#include "../TTUtils.h"

#include "Opening.h"

namespace tt
{

constexpr auto SCALE_PLAYER = 1.50f;
constexpr auto SCALE_BACKGROUND = 2.25f;

constexpr auto BOUNDARY_LEFT = 5.0f;
constexpr auto BOUNDARY_RIGHT = 2330.0f;
constexpr auto BOUNDARY_TOP = 10.0f;
constexpr auto BOUNDARY_BOTTOM = 4390.0f;

constexpr auto STEPSIZE = 16u;
    
Opening::Opening(ResourceManager& resmgr, sf::RenderTarget& target)
    : Scene(resmgr, target)
{
    sf::Texture temptext = *(_resources.load<sf::Texture>("maps/tucson.png"));
    _background = std::make_shared<Background>(temptext);
    _background->setScale(SCALE_BACKGROUND, SCALE_BACKGROUND);

    auto top = (_background->texture().getSize().y * 0.7f) - _window.getSize().y;
    sf::View view(sf::FloatRect(0.f, top, 
        static_cast<float>(_window.getSize().x), static_cast<float>(_window.getSize().y)));
    _window.setView(view);
    
    temptext = *(_resources.load<sf::Texture>("textures/tommy.png"));
    _player = std::make_shared<AnimatedSprite>(temptext, sf::Vector2i{ 64, 64 });
    _player->texture().setSmooth(true);
    _player->setSource(0,2);
    _player->setScale(SCALE_PLAYER, SCALE_PLAYER);
    auto [playerx, playery] = _player->getPosition();
    playerx = (_window.getSize().x / 2) - static_cast<float>(_player->getTextureRect().width);
    playery = (_window.getSize().y - (_player->getTextureRect().height * SCALE_PLAYER)) - 5;
    _player->setPosition(playerx, playery);

    _player->setAnimeCallback(
        [this]() 
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                auto [x, y] = _player->getPosition();
                if ((x - STEPSIZE) >= BOUNDARY_LEFT)
                {
                    _player->setPosition(x - STEPSIZE, y);
                    adjustView();
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                auto [x, y] = _player->getPosition();
                if ((x + STEPSIZE) <= BOUNDARY_RIGHT)
                {
                    _player->setPosition(x + STEPSIZE, y);
                    adjustView();
                }
            }
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                auto [x, y] = _player->getPosition();
                if ((y - STEPSIZE) >= BOUNDARY_TOP)
                {
                    _player->setPosition(x, y - STEPSIZE);
                    adjustView();
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                auto [x, y] = _player->getPosition();
                if ((y + STEPSIZE) <= BOUNDARY_BOTTOM)
                {
                    _player->setPosition(x, y + STEPSIZE);
                    adjustView();
                }
            }
        }
    );

    addUpdateable(_player);    

    // the order in which we add everything to the draw'able
    // vector is important, so we do it all at the end of
    // the function
    addDrawable(_background);
    addDrawable(_player);

    _debugText = std::make_shared<sf::Text>("", debugFont());
    _debugText->setFillColor(sf::Color::Red);
    addDrawable(_debugText);

    // after everything has been added, update our view
    adjustView();
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
                    < _background->getTextureRect().width * SCALE_BACKGROUND)
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
                    < _background->getTextureRect().height * SCALE_BACKGROUND)
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

    auto [playerXpos, playerYpos] = _player->getPosition();
    auto posText = fmt::format("PLAYER X:{} Y:{}", playerXpos, playerYpos);
    _debugText->setString(posText);

    auto [xpos, ypos] = _window.getView().getCenter();
    xpos -= ((_window.getSize().x / 2) - 10);
    ypos -= ((_window.getSize().y / 2) - 10);
    _debugText->setPosition(xpos, ypos);

    Scene::timestep();
    return 0;
}

void Opening::adjustView()
{
    const auto xpos = (_player->getPosition().x 
        + ((_player->getTextureRect().width * SCALE_PLAYER) / 2));

    const auto ypos = (_player->getPosition().y
        + ((_player->getTextureRect().height * SCALE_PLAYER) / 2));
    
    auto view = _window.getView();
    auto totalWidth = _background->getTextureRect().width * SCALE_BACKGROUND;
    if (xpos >= (_window.getSize().x / 2)
        && xpos <= (totalWidth - (_window.getSize().x / 2)))
    {
        view.setCenter(xpos, view.getCenter().y);
        _window.setView(view);
    }

    view = _window.getView();
    auto totalHeight = _background->getTextureRect().height * SCALE_BACKGROUND;
    if (ypos >= (_window.getSize().y / 2)
        && ypos <= (totalHeight - (_window.getSize().y / 2)))
    {
        view.setCenter(view.getCenter().x, ypos);
        _window.setView(view);
    }
}

} // namespace tt