#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include "LoadingScreen.h"

using namespace std::string_literals;

namespace tt
{

const std::vector<std::string> StatusTexts
{
    "examining taint vectors"s,
    "filing lawsuits"s,
    "rendering full eggsacks, dad"s,
    "preparing to call fbi"s,
    "tracking rogue government psy-ops cyberwarriors"s,
    "securing privacy by misappropriation and defamation under false light"s,
    "preparing guest room for the ascended ancestors"s,
    "siccing hackers on troll sites"s,
    "calling captain dufy"s,
    "loading demon spawn wireframes"s,
    "putting water in the gas tank"s,
    "writing congressman about lowering age of consent"s,
    "logging gender dysphoria with CPTSD due to a series of childhood traumas"s,
    "repairing spleen injuries"s,
    "loading rolling papers and $50 weed"s,
    "normalizing sax audio files"s,
    "updating gender assignment"s,
    "researching the dunning kruger effect"s,
    "rage posting on kiwifarms"s,
    "rage posting on AMB"s,
    "initializing communication with planetary management"s,
    "contacting microsoft tech support"s,
    "clearing cache of family pictures"s,
    "contacting Region 16, the traveling family"s,
    "initializing shart vectors"s,
    "modifying avatar gender assignment"s,
    "optimizing dumpster looting system"s,
    "loading arrest warrant for hoffman and associates"s,
    "informing website hosts of mean things said on their servers"s,
    "initializing prescribes hormones"s,
    "initializing gateway 2000 vga webcam"s
};

LoadingScreen::LoadingScreen(ResourceManager& res, sf::RenderTarget& target)
    : Screen(res, target),
      _logger { log::initializeLogger("LoadingScreen") }
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

    _statusTxt = std::make_shared<sf::Text>(""s, _monofont);
    _statusTxt->setCharacterSize(20);
    _statusTxt->setFillColor(sf::Color::White);
    _statusTxt->setPosition(static_cast<float>(_window.getSize().x / 3), 300);

    addDrawable(_statusTxt);
    addDrawable(textobj);

    updateStatusText(*(tt::select_randomly(StatusTexts)));

    _worker = std::thread(&tt::LoadingScreen::loadGameScreen, this);
    _lastUpdate = std::chrono::system_clock::now();
}

ScreenAction LoadingScreen::update()
{
    namespace ch = std::chrono;

    if (_loadingComplete)
    {
        _worker.join();
        return { ScreenActionType::CHANGE_GAMESCREEN, _gameScreen };
    }
    
    const auto nowt = ch::system_clock::now();
    const auto mst = ch::milliseconds{ tt::RandomNumber(1500, 3250) };
    if (ch::duration_cast<ch::milliseconds>(nowt - _lastUpdate) > mst)
    {
        updateStatusText(*(tt::select_randomly(StatusTexts)));
        _lastUpdate = nowt;
    }

    return {};
}

void LoadingScreen::updateStatusText(const std::string& text)
{
    _statusTxt->setString(text);
    const auto width = _statusTxt->getGlobalBounds().width;
    if (width > _window.getSize().x)
    {
        _statusTxt->setPosition(1, _statusTxt->getPosition().y);
    }
    else
    {
        const auto posx = (_window.getSize().x / 2) - (width / 2);
        _statusTxt->setPosition(posx, _statusTxt->getPosition().y);
    }
}

void LoadingScreen::loadGameScreen()
{
    sf::Clock timer;
    _gameScreen = std::make_shared<GameScreen>(_resources, _window);
    sf::Time elp = timer.getElapsedTime();
    _logger->debug("Game screen took {} milliseconds", elp.asMilliseconds());

    _loadingComplete = true;
}

} // namespace tt