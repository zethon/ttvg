#include "Scene.h"

namespace tt
{

Scene::Scene(ResourceManager& res, sf::RenderTarget& target, PlayerPtr player)
    : Screen(res, target),
      _weakPlayer{ player }
{
}

void Scene::enter()
{
    assert(!_player);
    _player = _weakPlayer.lock();
    _player->setPosition(_lastPlayerPos);
}

void Scene::exit()
{
    assert(_player);
    _lastPlayerPos = _player->getPosition();
    _player.reset();
}

bool Scene::walkPlayer(std::uint32_t stepsize)
{
    const auto stepSize = stepsize
        + (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ? 20 : 0);

    bool moved = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        auto xx = _player->getGlobalLeft();
        auto[x, y] = _player->getPosition();
        assert(xx >= 0);
        if (xx == 0) return false;

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
        if (x == boundaryRight) return false;

        x += stepSize;
        if (x > boundaryRight) x = boundaryRight;
        _player->setGlobalRight(x);
        moved = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        auto y = _player->getGlobalTop();
        assert(y >= 0);
        if (y == 0) return false;

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
        if (y == boundaryBottom) return false;

        y += stepSize;
        if (y > boundaryBottom)
        {
            y = boundaryBottom;
        }
        _player->setGlobalBottom(y);
        moved = true;
    }

    return moved;
}

} // namespace tt