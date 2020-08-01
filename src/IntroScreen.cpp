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
    // first add all the items keeping track of the
    // largest one
    for (const auto& text : textlist)
    {
        auto temp = std::make_shared<sf::Text>(text, font);
        menuItems.push_back(temp);
    }

    auto largest = std::max_element(menuItems.begin(), menuItems.end(),
        [](TextPtr a, TextPtr b) 
        { 
            return a->getLocalBounds().width < b->getLocalBounds().width; 
        });

    assert(largest != menuItems.end());

    auto xpos = window.getSize().x - ((*largest)->getLocalBounds().width + 100.f);
    auto ypos = 100.0f;

    for (const auto& item : menuItems)
    {
        item->setPosition(xpos, ypos);
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
    
    if (auto temp = _resources.load<sf::Font>("fonts/hobo.ttf"); 
        temp.has_value())
    {
        _font = *temp;
    }
    else
    {
        throw std::runtime_error("hobo.ttf could not be loaded!");
    }

    auto textobj = std::make_shared<sf::Text>("Lord of the Dumpsters", _font);
    textobj->setPosition(690, 10);
    textobj->setFillColor(sf::Color(255, 215, 9));

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
    _bgsong->setLoop(true);
    _bgsong->play();

    createMenu(_menuItems, { "Play Game", "Shart", "Exit Game" }, _window, _font);
    updateMenu(_selected, _menuItems);
    for (const auto& item : _menuItems)
    {
        addDrawable(item);
    }

    addDrawable(_sprite);
    addDrawable(textobj);

    // load sounds
    _selectorBuffer = _resources.loadPtr<sf::SoundBuffer>("sounds/selector.wav");
    _twkBuffer = _resources.loadPtr<sf::SoundBuffer>("sounds/tomwillkill.wav");
}

ScreenAction IntroScreen::poll(const sf::Event& e)
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
    else if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Space)
    {
        switch (_selected)
        {
            default:
            break;

            case 0: // play game
            {
                return { ScreenActionType::CHANGE_SCREEN, SCREEN_GAME };
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

    _font = *(_resources.load<sf::Font>("fonts/hobo.ttf"));
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

ScreenAction SplashScreen::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyReleased
            && e.key.code == sf::Keyboard::Space)
    {
        return { ScreenActionType::CHANGE_SCREEN, SCREEN_INTRO };
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
