#include "IntroScreen.h"

#include <cassert>
#include <thread>
#include <iostream>

namespace tt
{

namespace
{


void createMenu(TextList& menuItems,
    const std::vector<std::string>& textlist,
    sf::RenderTarget& window, 
    sf::Font& font)
{

    for (const auto& text : textlist)
    {
        auto temp = std::make_shared<sf::Text>(text, font);
        menuItems.push_back(temp);
    }

    auto ypos = 300.0f;

    for (const auto& item: menuItems)
    {
        item->setCharacterSize(40);

        //
        // Calculate xpos (right align)
        //
        auto winWidth       = window.getSize().x;
        auto itemWidth      = item->getLocalBounds().width;
        auto xpos           = (winWidth - itemWidth) - 50.f;

        item->setPosition(xpos, ypos);
       
        //
        // Increment ypos to the next item position
        //
        ypos += item->getLocalBounds().height + 20.0f;
    }

}

void updateMenu(std::uint16_t selection, TextList& menuItems)
{
    std::for_each(menuItems.begin(), menuItems.end(),
        [](TextPtr item) { item->setFillColor(sf::Color(64, 64, 64)); });

    if (selection < menuItems.size())
    {
        menuItems.at(selection)->setFillColor(sf::Color::Yellow);
    }
}

}

IntroScreen::IntroScreen(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target)
{
    
    if (auto temp = _resources.load<sf::Font>("fonts/pricedown.ttf"); 
        temp.has_value())
    {
        _font = *temp;
    }
    else
    {
        throw std::runtime_error("fonts/pricedown.ttf could not be loaded!");
    }

    //
    // This textobj is the title text
    //
    auto textobj = std::make_shared<sf::Text>(
                    "The Tommy Tucson\nVideo Game", _font);

    textobj->setCharacterSize(70);
    textobj->setFillColor(sf::Color(255, 215, 9));

    //
    // Get coords for title from window size. (i.e. xpos)
    //
    auto winWidth       = _window.getSize().x;
    auto titleWidth     = textobj->getLocalBounds().width;
    auto titleXpos      = (winWidth - titleWidth) - 25.f;

    //
    // Set coords for title text
    //
    textobj->setPosition(titleXpos - 20, 10);

    //
    // Load the scrolling Tom image.
    //
    auto bgt = _resources.load<sf::Texture>("images/tommy-1.png");

    if (!bgt)
    {
        throw std::runtime_error("tommy-1.png could not be loaded!");
    }

    _bgt = *bgt; // copy!!!
    _bgt.setRepeated(true);

    _sprite = std::make_shared<sf::Sprite>();
    _sprite->setTexture(_bgt);
    
    float xpos = 0;
    float ypos = (_bgt.getSize().y - _window.getSize().y) * -1.0f;

    _sprite->setPosition(xpos, ypos);
    
    _bgsong = _resources.openUniquePtr<sf::Music>("music/intro.wav");

    if (!_bgsong)
    {
        throw std::runtime_error("music/intro.mp3 could not be loaded!");
    }

    //
    // Add the scrolling Tom image.
    //
    addDrawable(_sprite);
    _bgsong->setLoop(true);
    _bgsong->play();

    //
    // Add the title text
    //
    addDrawable(textobj);

    //
    // Add the menu items
    //
    createMenu(_menuItems, 
        { 
            "Play Game", 
            "Settings", 
            "Exit Game" 
        }, _window, _font);

    updateMenu(_selected, _menuItems);

    for (const auto& item: _menuItems)
    {
        addDrawable(item);
    }

    //
    // load sounds
    //
    _selectorBuffer = 
        _resources.loadPtr<sf::SoundBuffer>("sounds/selector.wav");
    _twkBuffer = _resources.loadPtr<sf::SoundBuffer>("sounds/tomwillkill.wav");

}

PollResult IntroScreen::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Up)
    {
        if (_selected > 0)
        {
            _selected--;
            updateMenu(_selected, _menuItems);

            _tomWillKillSound.setBuffer(*_selectorBuffer);
            _tomWillKillSound.play();
        }
    }
    else if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Down)
    {
        if (_selected < _menuItems.size() - 1)
        {
            _selected++;
            updateMenu(_selected, _menuItems);

            _tomWillKillSound.setBuffer(*_selectorBuffer);
            _tomWillKillSound.play();
        }
    }
    else if (e.type == sf::Event::KeyPressed
        && (e.key.code == sf::Keyboard::Space
            || e.key.code == sf::Keyboard::Enter))
    {
        switch (_selected)
        {
            default:
            break;

            case 0: // play game
            {
                return {true, { ScreenActionType::CHANGE_SCREEN, SCREEN_GAME }};
            }

            case 2: // exit
            {
                _bgsong->stop();

                _tomWillKillSound.setBuffer(*_twkBuffer);
                _tomWillKillSound.setLoop(false);
                _tomWillKillSound.play();
                _tomWillKillSound.setVolume(50);

                while (_tomWillKillSound.getStatus() == sf::Sound::Playing)
                {
                    std::this_thread::yield();
                }

                // super hack! I'm too lazy and don't care enough right now
                sf::RenderWindow* window = dynamic_cast<sf::RenderWindow*>(&_window);
                window->close();
            }
            break;
        }
    }

    return {};
}

ScreenAction IntroScreen::timestep()
{
    auto[x, y] = _sprite->getPosition();

    if (auto elapsed = _clock.getElapsedTime();
        elapsed.asMilliseconds() > 15)
    {
        y += 5;
        if (y > _window.getSize().y)
        {
            y = _bgt.getSize().y * -1.0f;
        }

        _sprite->setPosition(x, y);
        _clock.restart();
    }

    return {};
}

void IntroScreen::close()
{
    Screen::close();
    _bgsong->stop();
}

SplashScreen::SplashScreen(ResourceManager& res, sf::RenderTarget& target)
    : Screen(res, target)
{
    constexpr auto bgXScale = 0.33f;
    constexpr auto bgYScale = 0.33f;

    _bg = *(_resources.load<sf::Texture>("images/splash.jpg"));
    auto sprite = std::make_shared<sf::Sprite>(_bg);
    sprite->setScale(bgXScale, bgYScale);
    
    auto xpos = (_window.getSize().x / 2) - ((_bg.getSize().x * bgXScale) / 2);
    auto ypos = (_window.getSize().y / 2) - ((_bg.getSize().y * bgYScale) / 2);
    sprite->setPosition(xpos, ypos - 30);
    
    addDrawable(sprite);

    _font = *(_resources.load<sf::Font>("fonts/pricedown.ttf"));

    auto logoText = std::make_shared<sf::Text>("Dog Finger Studios", _font);
    logoText->setCharacterSize(100);

    auto txpos = (_window.getSize().x / 2) - ((logoText->getLocalBounds().width) / 2);
    logoText->setPosition(txpos, _window.getSize().y - 150.0f);

    addDrawable(logoText);

    _twkBuffer = _resources.loadPtr<sf::SoundBuffer>("sounds/tomwillkill.wav");
    _tomWillKillSound.setBuffer(*_twkBuffer);
    _tomWillKillSound.setVolume(50);
    _tomWillKillSound.play();

    _clock.restart();
}

PollResult SplashScreen::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyReleased
            && e.key.code == sf::Keyboard::Space)
    {
        return {true, { ScreenActionType::CHANGE_SCREEN, SCREEN_INTRO }};
    }

    return {};
}

ScreenAction SplashScreen::timestep()
{
    if (auto elapsed = _clock.getElapsedTime();
        elapsed.asMilliseconds() > 1500)
    {
        _tomWillKillSound.stop();
        return { ScreenActionType::CHANGE_SCREEN, SCREEN_INTRO };
    }

    return {};
}

} // namespace tt
