#include "LoadingScreen.h"

namespace tt
{

LoadingScreen::LoadingScreen(ResourceManager& res, sf::RenderTarget& target)
    : Screen(res, target)
{
    // I guess I was worried about error handling at one point!
    if (auto temp = _resources.load<sf::Font>("fonts/hobo.ttf");
        temp.has_value())
    {
        _font = *temp;
    }
    else
    {
        throw std::runtime_error("hobo.ttf could not be loaded!");
    }

    if (auto temp2 = _resources.load<sf::Font>("fonts/mono_bold.ttf");
        temp2.has_value())
    {
        _monofont = *(temp2);
    }
    else
    {
        throw std::runtime_error("mono.ttf could not be loaded!");
    }

    auto textobj = std::make_shared<sf::Text>("Loading", _font);
    textobj->setCharacterSize(150);
    textobj->setFillColor(sf::Color::Red);
    auto x = (_window.getSize().x / 2) - (textobj->getGlobalBounds().width / 2);
    textobj->setPosition(x, 30);

    auto lsinktxt = std::make_shared<sf::Text>("THIS IS A TEST", _monofont);
    lsinktxt->setCharacterSize(20);
    lsinktxt->setFillColor(sf::Color::White);
    lsinktxt->setPosition(_window.getSize().x / 3, 300);

    _logsink = std::make_shared<SFTextLogSink>(lsinktxt, target.getSize().x);

    _originalLogLevel = spdlog::get_level();
    tt::log::rootLogger()->sinks().push_back(_logsink);

    addDrawable(lsinktxt);
    addDrawable(textobj);

    _worker = std::thread(&tt::LoadingScreen::loadGameScreen, this);
}

ScreenAction LoadingScreen::timestep()
{
    if (_loadingComplete)
    {
        _worker.join();
        return {ScreenActionType::CHANGE_GAMESCREEN, _gameScreen};
    }

    return {};
}

void LoadingScreen::loadGameScreen()
{
    _gameScreen = std::make_shared<GameScreen>(_resources, _window);
    _loadingComplete = true;
}

void LoadingScreen::close()
{
    auto logsinkit = std::find_if(tt::log::rootLogger()->sinks().begin(),
                              tt::log::rootLogger()->sinks().end(),
                              [this](auto other) { return other == _logsink; });

    assert(logsinkit != tt::log::rootLogger()->sinks().end());
    tt::log::rootLogger()->sinks().erase(logsinkit);
    spdlog::set_level(_originalLogLevel);
}

} // namespace tt