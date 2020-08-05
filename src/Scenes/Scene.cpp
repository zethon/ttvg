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

} // namespace tt