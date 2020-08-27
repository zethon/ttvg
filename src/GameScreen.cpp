#include "Scenes/Scene.h"
#include "Scenes/Opening.h"

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
    _scenes.emplace("tucson", std::make_shared<Opening>(resmgr, target, _player));
    _scenes.emplace("EuclidHouse", std::make_shared<Scene>("EuclidHouse", resmgr, target, _player));

    _scenes.emplace(
                "Hospital", 
                std::make_shared<Scene>("Hospital", resmgr, target, _player));

    _scenes.emplace(
                "CourthouseInterior", 
                std::make_shared<Scene>("CourthouseInterior", 
                                        resmgr, 
                                        target, 
                                        _player)
    );

    _currentScene = _scenes["tucson"];

    // make sure all constructors across all scenes
    // have been run BEFORE init()'ing the scenes
    for (auto& item : _scenes)
    {
        item.second->init();
    }

    _currentScene->enter();
}

void GameScreen::draw()
{
    _currentScene->draw();
}

PollResult GameScreen::poll(const sf::Event& e)
{
    assert(_currentScene);
    auto result = _currentScene->poll(e);

    switch (result.action.type)
    {
        default:
        break;

        case ScreenActionType::CHANGE_SCREEN:
        break;

        case ScreenActionType::CHANGE_SCENE:
        {
            const auto name = boost::any_cast<std::string>(result.action.data);
            assert(_scenes.find(name) != _scenes.end());

            _currentScene->exit();
            _currentScene = _scenes.at(name);
            _currentScene->enter();
        }
        break;
    }

    return result;
}

ScreenAction GameScreen::timestep()
{
    assert(_currentScene);
    return _currentScene->timestep();
}

} // namespace tt
