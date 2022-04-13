#include "GameOverScreen.h"

namespace tt
{

GameOverScreen::GameOverScreen(ResourceManager& res, sf::RenderTarget& target)
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

    auto textobj = std::make_shared<sf::Text>("Game Over", _font);
    textobj->setCharacterSize(150);

    textobj->setFillColor(sf::Color::Red);

    auto x = (_window.getSize().x / 2) - (textobj->getGlobalBounds().width / 2);
    textobj->setPosition(x, 30);
    
    addDrawable(textobj);
}

PollResult GameOverScreen::poll(const sf::Event & e)
{
    if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Space)
    {
        return {true, { ScreenActionType::CHANGE_SCREEN, SCREEN_INTRO }};
    }

    return {};
}

} // namespace tt