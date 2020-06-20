#include "IntroScreen.h"

namespace tt
{

IntroScreen::IntroScreen(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target)
{
    auto temp = _resources.load<sf::Font>("hobo.ttf");
    if (!temp.has_value())
    {
        throw std::runtime_error("hobo.ttf could not be loaded!");
    }

    _font = *temp; // copy?

    const std::string text = "Hello Hobo!";

    auto textobj = std::make_shared<sf::Text>(text, _font);

    //auto width = sf::VideoMode::getDesktopMode().width;
    //auto height = sf::VideoMode::getDesktopMode().height;

    auto bgt = _resources.load<sf::Texture>("images/tommy-1.png");
    if (!bgt)
    {
        throw std::runtime_error("tommy-1.png could not be loaded!");
    }

    _bgt = *bgt; // copy!!!
    _sprite = std::make_shared<sf::Sprite>();
    _sprite->setTexture(_bgt);
    _sprite->setPosition(0, 10);

    addDrawable(_sprite);
    addDrawable(textobj);
}

void IntroScreen::poll()
{
    auto [x, y] = _sprite->getPosition();

    if (auto elapsed = _clock.getElapsedTime();
        elapsed.asMilliseconds() > 10)
    {
        y += 1;
        if (y > 640)
        {
            y = -640;
        }

        _sprite->setPosition(x, y + 5);
        _clock.restart();
    }
}

} // namespace tt