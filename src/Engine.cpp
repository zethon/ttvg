#include "Engine.h"
#include "IntroScreen.h"
#include "GameScreen.h"

namespace tt
{

TooterEngine::TooterEngine(const boost::filesystem::path& respath, RenderTargetPtr render)
    : _resourceManager{ respath },
      _renderTarget { render }
{
    _currentScreen = std::make_shared<IntroScreen>(_resourceManager, *_renderTarget);
}

void TooterEngine::drawScreen()
{
    if (_currentScreen)
    {
        _currentScreen->draw();
    }
}

void TooterEngine::poll(const sf::Event& e)
{
    if (auto id = _currentScreen->poll(e); id != 0)
    {
        changeScreen(id);
    }
}

void TooterEngine::timestep()
{
    if (auto id = _currentScreen->timestep(); id != 0)
    {
        changeScreen(id);
    }
}

void TooterEngine::changeScreen(std::uint16_t id)
{
    switch (id)
    {
        default:
        break;

        case SCREEN_INTRO:
            _currentScreen->close();
            _currentScreen.reset();
            _currentScreen = std::make_shared<IntroScreen>(_resourceManager, *_renderTarget);
        break;

        case SCREEN_GAME:
            _currentScreen->close();
            _currentScreen.reset();
            _currentScreen = std::make_shared<GameScreen>(_resourceManager, *_renderTarget);
        break;
    }
}

} // namespace tt