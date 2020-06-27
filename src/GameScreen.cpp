#include "Scenes/Opening.h"

#include "GameScreen.h"

namespace tt
{

GameScreen::GameScreen(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target),
      _currentScene { std::make_unique<Opening>(resmgr, target) }
{
}

void GameScreen::draw()
{
    _currentScene->draw();
}

std::uint16_t GameScreen::poll(const sf::Event& e)
{
    assert(_currentScene);
    _currentScene->poll(e);
    return 0;
}

std::uint16_t GameScreen::timestep()
{
    assert(_currentScene);
    _currentScene->timestep();

    return Screen::timestep();
}

} // namespace tt