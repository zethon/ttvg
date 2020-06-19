#pragma once
#include <memory>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"
#include "Screen.h"

namespace tt
{

class TooterEngine
{
    ResourceManager             _resourceManager;
    std::shared_ptr<Screen>     _currentScreen;
    
public:
    TooterEngine(const std::filesystem::path& respath)
        : _resourceManager{ respath }
    {
        _currentScreen = std::make_shared<IntroScreen>(_resourceManager);
    }

    void drawScreen(sf::RenderWindow& window)
    {
        if (_currentScreen)
        {
            _currentScreen->draw(window);
        }
    }

    void poll()
    {
        _currentScreen->poll();
    }
};

} // namespace tt