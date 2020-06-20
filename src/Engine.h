#pragma once
#include <memory>
#include <filesystem>

#include <SFML/Graphics.hpp>

#include "ResourceManager.h"
#include "Screen.h"

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
    void poll(const sf::Event& e);
    void timestep();

private:
    void changeScreen(std::uint16_t id);

};

} // namespace tt