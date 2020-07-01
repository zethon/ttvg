#include <boost/range/adaptor/indexed.hpp>

#include <fmt/core.h>

#include "../TTUtils.h"

#include "Opening.h"

namespace tt
{

constexpr auto SCALE_PLAYER = 1.50f;
constexpr auto SCALE_BACKGROUND = 2.25f;

constexpr auto STEPSIZE = 16u;

constexpr auto PLAYER_START_X = 1616.0f;
constexpr auto PLAYER_START_Y = 2875.0f;
    
Opening::Opening(ResourceManager& resmgr, sf::RenderTarget& target)
    : Scene(resmgr, target)
{
    sf::Texture temptext = *(_resources.load<sf::Texture>("maps/tucson.png"));
    _background = std::make_shared<Background>(temptext);
    _background->setScale(SCALE_BACKGROUND, SCALE_BACKGROUND);
    _background->setPosition(0.0f, 0.0f);

    auto top = (_background->texture().getSize().y * 0.7f) - _window.getSize().y;
    sf::View view(sf::FloatRect(0.f, top, 
        static_cast<float>(_window.getSize().x), static_cast<float>(_window.getSize().y)));
    _window.setView(view);
    
    temptext = *(_resources.load<sf::Texture>("textures/tommy.png"));
    _player = std::make_shared<Player>(temptext, sf::Vector2i{ 64, 64 });
    _player->texture().setSmooth(true);
    _player->setSource(0,2);
    _player->setScale(SCALE_PLAYER, SCALE_PLAYER);
    _player->setPosition(PLAYER_START_X, PLAYER_START_Y);

    _player->setAnimeCallback(
        [this]() 
        {
            _debugText->setString("");
            const auto stepSize = STEPSIZE 
                + (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
                    || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ? 20 : 0);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                const auto boundaryLeft = _background->getLeftBoundary();
                auto [x, y] = _player->getPosition();
                assert(x >= boundaryLeft);
                if (x == boundaryLeft) return;

                x -= stepSize;
                if (x < boundaryLeft) x = boundaryLeft;
                _player->setPosition(x, y);
                adjustView();
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                const auto boundaryRight = _background->getRightBoundary() 
                    - _player->getGlobalBounds().width;

                auto [x, y] = _player->getPosition();
                assert(x <= boundaryRight);
                if (x == boundaryRight) return;

                x += stepSize;
                if (x > boundaryRight) x = boundaryRight;
                _player->setPosition(x, y);
                adjustView();
            }
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                const auto boundaryTop = _background->getTopBoundary();
                auto [x, y] = _player->getPosition();
                assert(y >= boundaryTop);
                if (y == boundaryTop) return;

                y -= stepSize;
                if (y < boundaryTop) y = boundaryTop;
                _player->setPosition(x, y);
                adjustView();
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                const auto boundaryBottom = _background->getBottomBoundary() 
                    - (_player->getGlobalBounds().height + 32);

                auto [x, y] = _player->getPosition();
                assert(y <= boundaryBottom);
                if (y == boundaryBottom) return;

                y += stepSize;
                if (y > boundaryBottom)
                {
                    y = boundaryBottom;
                }
                _player->setPosition(x, y);
                adjustView();
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
    _debugText->setFillColor(sf::Color::White);
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

    auto [playerXpos, playerYpos] = _player->getPosition();

    auto [tilex, tiley] = tt::getTileXY(_player->getPosition(), { 16, 16 });
    auto posText = fmt::format("PLAYER X:{} Y:{}\nTILE X:{} Y:{}", 
        playerXpos, playerYpos, tilex, tiley);

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