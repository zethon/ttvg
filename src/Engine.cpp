#include "Engine.h"

namespace tt
{

TooterEngine::TooterEngine(const std::filesystem::path& respath, RenderTargetPtr render)
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
    _currentScreen->poll(e);
}

void TooterEngine::timestep()
{
    _currentScreen->timestep();
}

} // namespace tt