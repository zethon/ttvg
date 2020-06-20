#include "IntroScreen.h"

#include <iostream>

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
    
    float xpos = 0;
    std::int32_t ypos = (_bgt.getSize().y - _window.getSize().y) * -1;
    _sprite->setPosition(xpos, ypos);

    auto[x1, y1] = _sprite->getPosition();
    std::cout << "!! (" << x1 << "),(" << y1 << ")\n";


    _bgsong = _resources.openUniquePtr<sf::Music>("music/intro.wav");
    if (!_bgsong)
    {
        throw std::runtime_error("music/intro.mp3 could not be loaded!");
    }
    _bgsong->setLoop(true);
    _bgsong->play();

    addDrawable(_sprite);
    addDrawable(textobj);
}

void IntroScreen::poll()
{
    auto [x, y] = _sprite->getPosition();

    if (auto elapsed = _clock.getElapsedTime();
        elapsed.asMilliseconds() > 15)
    {
        y += 5;
        if (y > _window.getSize().y)
        {
            y = static_cast<std::int32_t>((_bgt.getSize().y - _window.getSize().y) * -1);
        }

        _sprite->setPosition(x, y);
        _clock.restart();
    }

    auto[x1, y1] = _sprite->getPosition();
    std::cout << "-- (" << x1 << "),(" << y1 << ")\n";
}

} // namespace tt