#include "Scenes/Opening.h"
#include "Scenes/EuclidHouse.h"

#include "GameScreen.h"

namespace tt
{

GameScreen::GameScreen(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target)
{
    // the `Player` object is shared among all the `Scene` objects
    auto textptr = _resources.cacheTexture("textures/tommy.png");
    assert(textptr);
    textptr->setSmooth(true);
    _player = std::make_shared<Player>(*textptr, sf::Vector2i{ 64, 64 });

    // TODO: as the game grows these constructions will take longer
    // and should probably be done in parallel and/or with a loading screen
    _scenes2.emplace("tucson", std::make_shared<Opening>(resmgr, target, _player));
    _scenes2.emplace("EuclidHouse", std::make_shared<EuclidHouse>(resmgr, target, _player));

    _currentScene = _scenes2["tucson"];

    // make sure all constructors across all scenes
    // have been run BEFORE init()'ing the scenes
    for (auto& scene : _scenes)
    {
        scene->init();
    }

    _currentScene->enter();
}

void GameScreen::draw()
{
    _currentScene->draw();
}

ScreenAction GameScreen::poll(const sf::Event& e)
{
    assert(_currentScene);
    auto result = _currentScene->poll(e);
    switch (result.type)
    {
        default:
            return Screen::poll(e);

        case ScreenActionType::CHANGE_SCREEN:
        break;

        case ScreenActionType::CHANGE_SCENE:
        {
            const auto name = boost::any_cast<std::string>(result.data);
            assert(_scenes2.find(name) != _scenes2.end());

            _currentScene->exit();
            _currentScene = _scenes2.at(name);
            _currentScene->enter();
        }
        break;
    }

    return Screen::poll(e);
}

ScreenAction GameScreen::timestep()
{
    assert(_currentScene);
    return _currentScene->timestep();
}

} // namespace tt