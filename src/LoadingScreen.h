#pragma once

#include <thread>

#include <spdlog/sinks/base_sink.h>

#include "Screen.h"
#include "GameScreen.h"

namespace tt
{

class SFTextLogSink: public spdlog::sinks::sink
{

public:
    SFTextLogSink(std::shared_ptr<sf::Text> dst, unsigned int width)
        : _dst{dst},
        _windowwidth{width}
    {
        // nothing to do
    }

protected:
    void log(const spdlog::details::log_msg& msg) override
    {
        _dst->setString(fmt::to_string(msg.payload));
//        const auto width = _dst->getGlobalBounds().width;
//        if (width > _windowwidth)
//        {
//            _dst->setPosition(1, _dst->getPosition().y);
//        }
//        else
//        {
//            const auto posx = (_windowwidth / 2) - (width / 2);
//            _dst->setPosition(posx, _dst->getPosition().y);
//        }
    }

    void flush() override
    {
        _dst->setString({});
    }

    void set_pattern(const std::string&) override
    {
        // nothing to do
    }

    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override
    {
        // nothing to do
    }

private:
    std::shared_ptr<sf::Text>   _dst;
    unsigned int                _windowwidth;
};

class LoadingScreen : public Screen
{

public:
    LoadingScreen(ResourceManager& res, sf::RenderTarget& target);

    ScreenAction timestep() override;
    void close() override;

private:
    void loadGameScreen();

    sf::Font            _font;
    sf::Font            _monofont;
    std::thread         _worker;

    std::atomic_bool            _loadingComplete = false;
    std::shared_ptr<GameScreen> _gameScreen;

    tt::ItemInfo                _localPlayerInfo;
    tt::PlayerPtr               _localPlayer;

    std::shared_ptr<SFTextLogSink>  _logsink;
    spdlog::level::level_enum       _originalLogLevel;
};

} // namespace tt