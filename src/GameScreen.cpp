#include "Scenes/Opening.h"
#include "Scenes/EuclidHouse.h"

#include "GameScreen.h"

namespace tt
{

GameScreen::GameScreen(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target)
{
    auto textptr = _resources.cacheTexture("textures/tommy.png");
    assert(textptr);
    textptr->setSmooth(true);
    _player = std::make_shared<Player>(*textptr, sf::Vector2i{ 64, 64 });

    _scenes.emplace_back(std::make_unique<Opening>(resmgr, target, _player));
    _scenes.emplace_back(std::make_unique<EuclidHouse>(resmgr, target, _player));

    _scenes[_currentScene]->enter();
}

void GameScreen::draw()
{
    _scenes[_currentScene]->draw();
}

ScreenAction GameScreen::poll(const sf::Event& e)
{
    assert(_scenes[_currentScene]);
    auto result = _scenes[_currentScene]->poll(e);
    switch (result.type)
    {
        default:
            return Screen::poll(e);

        case ScreenActionType::CHANGE_SCREEN:
        break;

        case ScreenActionType::CHANGE_SCENE:
        {
            if (_scenes[result.data] != nullptr)
            {
                _scenes[_currentScene]->exit();
                _currentScene = result.data;
                _scenes[_currentScene]->enter();
            }
        }
        break;
    }

    return Screen::poll(e);
}

ScreenAction GameScreen::timestep()
{
    assert(_scenes[_currentScene]);
    _scenes[_currentScene]->timestep();
    return Screen::timestep();
}

} // namespace tt