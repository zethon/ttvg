#pragma once
#include <memory>
#include <boost/filesystem.hpp>

#include <SFML/Graphics.hpp>

#include "Screens/Screen.h"

#include "ResourceManager.h"
#include "TooterLogger.h"

namespace fs = boost::filesystem;

namespace tt
{

using RenderTargetPtr = std::shared_ptr<sf::RenderTarget>;

class TooterEngine
{
    ResourceManager     _resourceManager;
    RenderTargetPtr     _renderTarget;

    std::shared_ptr<Screen> _currentScreen;
    log::SpdLogPtr          _logger;
    
public:
    TooterEngine(
        const fs::path& respath,
        const amb::SettingsPtr& settings,
        RenderTargetPtr render);

    void initAudioService();
    void refreshAudioService();
    
    void drawScreen();
    PollResult poll(const sf::Event& e);
    void update();
    void changeScreen(std::uint16_t id);

};

} // namespace tt