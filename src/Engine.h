#pragma once
#include <memory>
#include <filesystem>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"
#include "Screen.h"
#include "IntroScreen.h"

namespace tt
{

using RenderTargetPtr = std::shared_ptr<sf::RenderTarget>;

class TooterEngine
{
    ResourceManager     _resourceManager;
    RenderTargetPtr     _renderTarget;

    std::shared_ptr<Screen>     _currentScreen;
    
public:
    TooterEngine(
        const std::filesystem::path& respath,
        RenderTargetPtr render);
    
    void drawScreen();
    void poll();
    void timestep();
};

} // namespace tt