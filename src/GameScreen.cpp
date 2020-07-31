#include "Scenes/Opening.h"
#include "Scenes/EuclidHouse.h"

#include "GameScreen.h"

namespace tt
{

GameScreen::GameScreen(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target)
{
    _scenes.emplace_back(std::make_unique<Opening>(resmgr, target));
    _scenes.emplace_back(std::make_unique<EuclidHouse>(resmgr, target));
}

void GameScreen::draw()
{
    _scenes[_currentScene]->draw();
}

std::uint16_t GameScreen::poll(const sf::Event& e)
{
    assert(_currentScene);
    auto result = _scenes[_currentScene]->poll(e);
    
    switch (result)
    {
        defaut:
        break;

        case ScreenAction::CHANGE_SCREEN:
            break;
    }

    return 0;
}

std::uint16_t GameScreen::timestep()
{
    _scenes[_currentScene]->timestep();
    return Screen::timestep();
}

} // namespace tt