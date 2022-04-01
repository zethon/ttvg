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
    "tracking ogue government psy-ops cyberwarriors"s,
    "securing privacy by misappropriation and defamation under false light"s,
    "publishing PI"s,
    "siccing hackers on troll sites"s,
    "calling captain dufy"s,
    "loading demon spawn wireframes"s,
    "putting water in the gas tank"s,
    "writing congressman about lowering age of consent"s,
    "logging gender dysphoria with CPTSD due to a series of childhood traumas"s,
    "repairing spleen injuries"s,
    "loading rolling papers and $50 weed"s,
    "normalizing sax audio files"s,
    "projecting fetishes"s,
    "reearching the dunning kruger effect"s
};

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

    _statusTxt = std::make_shared<sf::Text>(""s, _monofont);
    _statusTxt->setCharacterSize(20);
    _statusTxt->setFillColor(sf::Color::White);
    _statusTxt->setPosition(static_cast<float>(_window.getSize().x / 3), 300);

    addDrawable(_statusTxt);
    addDrawable(textobj);

    _statusWorker = std::thread(&tt::LoadingScreen::statusWork, this);
    _worker = std::thread(&tt::LoadingScreen::loadGameScreen, this);
}

ScreenAction LoadingScreen::timestep()
{
    if (_loadingComplete)
    {
        _worker.join();
        _statusWorker.join();
        return {ScreenActionType::CHANGE_GAMESCREEN, _gameScreen};
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

void LoadingScreen::statusWork()
{
    while (!_loadingComplete)
    {
        updateStatusText(*(tt::select_randomly(StatusTexts)));
        std::uint32_t seconds = tt::RandomNumber(500,1750);
        std::this_thread::sleep_for(std::chrono::milliseconds {seconds});
    }
}

void LoadingScreen::loadGameScreen()
{
    _gameScreen = std::make_shared<GameScreen>(_resources, _window);
    _loadingComplete = true;
}

} // namespace tt