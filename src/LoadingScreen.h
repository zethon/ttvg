#pragma once

#include <thread>

#include <spdlog/sinks/base_sink.h>

#include "Screen.h"
#include "GameScreen.h"

namespace tt
{

class LoadingScreen : public Screen
{

public:
    LoadingScreen(ResourceManager& res, sf::RenderTarget& target);

    ScreenAction timestep() override;

private:
    void loadGameScreen();
    void statusWork();
    void updateStatusText(const std::string& text);

    sf::Font            _font;
    sf::Font            _monofont;
    std::thread         _worker;

    std::thread             _statusWorker;

    std::atomic_bool            _loadingComplete = false;
    std::shared_ptr<GameScreen> _gameScreen;
    std::shared_ptr<sf::Text>   _statusTxt;
};

} // namespace tt