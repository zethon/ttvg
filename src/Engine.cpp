#include "Screens/IntroScreen.h"
#include "Screens/GameScreen.h"
#include "Screens/GameOverScreen.h"
#include "Screens/LoadingScreen.h"
#include "Screens/SettingsScreen.h"

#include "AudioService.h"
#include "Engine.h"

namespace tt
{

TooterEngine::TooterEngine(const boost::filesystem::path& respath, const amb::SettingsPtr& settings, RenderTargetPtr render)
    : _resourceManager{ respath, settings },
      _renderTarget { render },
      _logger { log::initializeLogger("Engine") }
{
    initAudioService();
    _currentScreen = std::make_shared<SplashScreen>(_resourceManager, *_renderTarget);
}

void TooterEngine::drawScreen()
{
    if (_currentScreen)
    {
        _currentScreen->draw();
    }
}

PollResult TooterEngine::poll(const sf::Event& e)
{
    if (auto result = _currentScreen->poll(e); 
        result.action.type == ScreenActionType::CHANGE_SCREEN)
    {
        _logger->debug("changing screen from poll response");
        changeScreen(boost::any_cast<std::uint16_t>(result.action.data));
    }
    else if (result.action.type == ScreenActionType::EXIT_GAME)
    {
        _logger->debug("exiting game from poll response");
        return { true, { ScreenActionType::EXIT_GAME }};
    }

    return {};
}

void TooterEngine::update()
{
    if (auto action = _currentScreen->update();
        action.type == ScreenActionType::CHANGE_SCREEN)
    {
        _logger->debug("changing screen from update result");
        changeScreen(boost::any_cast<std::uint16_t>(action.data));
    }
    else if (action.type == ScreenActionType::CHANGE_GAMESCREEN)
    {
        _logger->debug("loading game screen");

        refreshAudioService();

        auto gamescreen = boost::any_cast<std::shared_ptr<GameScreen>>(action.data);
        _currentScreen->close();
        _currentScreen.reset();
        _currentScreen = gamescreen;
    }
}

void TooterEngine::changeScreen(std::uint16_t id)
{
    _logger->info("changing to screen {}", id);

    refreshAudioService();

    switch (id)
    {
        default:
            _logger->critical("invalid screen id {}", id);
            throw std::runtime_error("invalid screen id");
        break;

        case SCREEN_INTRO:
        {
            _currentScreen->close();
            _currentScreen.reset();
            _currentScreen = std::make_shared<IntroScreen>(_resourceManager, *_renderTarget);
        }
        break;

        case SCREEN_GAME:
        {
            _currentScreen->close();
            _currentScreen.reset();
            _currentScreen = std::make_shared<GameScreen>(_resourceManager, *_renderTarget);
        }
        break;

        case SCREEN_GAMEOVER:
        {
            _currentScreen->close();
            _currentScreen.reset();
            _currentScreen = std::make_shared<GameOverScreen>(_resourceManager, *_renderTarget);
        }
        break;

        case SCREEN_LOADING:
        {
            _currentScreen->close();
            _currentScreen.reset();
            _currentScreen = std::make_shared<LoadingScreen>(_resourceManager, *_renderTarget);
        }
        break;

        case SCREEN_SETTINGS:
        {
            _currentScreen->close();
            _currentScreen.reset();
            _currentScreen = std::make_shared<SettingsScreen>(_resourceManager, *_renderTarget);
        }
        break;
    }
}

void TooterEngine::initAudioService()
{
    const auto settings = _resourceManager.settings();

    IAudioPtr musicptr;
    if (auto value = settings->value("audio.volume.music",100); value > 0)
    {
        musicptr = std::make_shared<tt::MusicAudio>(_resourceManager);
        musicptr->setAllVolume(value);
    }
    else
    {
        musicptr = std::make_shared<tt::NullAudio>();
    }

    if (settings->value("logs.music.enabled", false))
    {
        musicptr = std::make_shared<tt::LoggedAudio>(musicptr);
    }

    IAudioPtr soundptr;
    if (auto value = settings->value("audio.volume.sfx",100); value > 0)
    {
        soundptr = std::make_shared<tt::SfxAudio>(_resourceManager);
        soundptr->setAllVolume(value);
    }
    else
    {
        soundptr = std::make_shared<tt::NullAudio>();
    }

    if (settings->value("logs.sfx.enabled", false))
    {
        soundptr = std::make_shared<tt::LoggedAudio>(soundptr);
    }

    assert(musicptr);
    assert(soundptr);

    tt::AudioLocator::setMusic(musicptr);
    tt::AudioLocator::setSound(soundptr);
}

void TooterEngine::refreshAudioService()
{
    const auto settings = _resourceManager.settings();

    auto value = settings->value("audio.volume.music", 100);
    tt::AudioLocator::music()->setAllVolume(value);

    value = settings->value("audio.volume.sfx", 100);
    tt::AudioLocator::sound()->setAllVolume(value);
}

} // namespace tt