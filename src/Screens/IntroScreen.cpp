#include <cassert>
#include <thread>
#include <iostream>

#include <fmt/core.h>

#include "../TooterLogger.h"
#include "../AudioService.h"

#include "IntroScreen.h"

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
       
        item->setOutlineColor(sf::Color(0,0,0,255));
        item->setOutlineThickness(5);

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

IntroScreen::IntroScreen(       ResourceManager& resmgr, 
                                sf::RenderTarget& target )

                            :   Screen(resmgr, target)

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
                    "Tommy Tucson", _font);

    textobj->setCharacterSize(70);
    textobj->setFillColor(sf::Color(255, 215, 9));

    textobj->setOutlineColor(sf::Color(0,0,0,255));
    textobj->setOutlineThickness(5);

    //
    // Get coords for title from window size. (i.e. xpos)
    //
    auto winWidth       = _window.getSize().x;
    auto winHeight      = _window.getSize().y;
    auto titleWidth     = textobj->getLocalBounds().width;
    auto titleXpos      = (winWidth - titleWidth) - 25.f;

    //
    // Set coords for title text
    //
    textobj->setPosition(titleXpos - 20, 10);

    //
    // See https://en.sfml-dev.org/forums/index.php?topic=10276.0
    //
    _bgt.reserve(INTRO_IMAGES);

    //
    // Load background images
    //
    for(int i = 0; i < INTRO_IMAGES; i++)
    {
        //
        // Load the intro images.
        //
        auto filename = fmt::format("images/ttvg-intro-screen-{}.png", i+1);
      
        auto logger = log::initializeLogger("Intro");
        logger->debug("Loading intro file {}", filename);

        // auto bgt = _resources.load<sf::Texture>(filename);
        auto bgt = _resources.loadPtr<sf::Texture>(filename);

        if (!bgt)
        {
            throw std::runtime_error(filename);
        }

        _bgt.push_back(std::move(*bgt));
        // _bgt.push_back(*bgt);
        
        auto sprite = std::make_shared<sf::Sprite>();

        sprite->setTexture(_bgt.at(i));

        sprite->setPosition(0, 0);
        sprite->setColor(sf::Color(0, 0, 0, 0));

        //
        // Fit this image to the window size.
        //
        sprite->setScale(
                winWidth    / sprite->getLocalBounds().width,
                winHeight   / sprite->getLocalBounds().height );
 
        //
        // Add this intro image.
        //
        addDrawable(sprite);

        _sprite.push_back(sprite);

    }

    auto music = tt::AudioLocator::music();
    music->cacheAudio(BACKGROUND_SONG);

    music->setLoop(BACKGROUND_SONG, true);
    music->play(BACKGROUND_SONG);

    //
    // Add the title text
    //
    addDrawable(textobj);

    //
    // Add the menu items
    //
    createMenu(_menuItems, 
        { 
            "New Game",
            "Load Game",
            "Settings", 
            "Exit Game" 
        }, _window, _font);

    updateMenu(_selected, _menuItems);

    for (const auto& item: _menuItems)
    {
        addDrawable(item);
    }

    tt::AudioLocator::sound()->cacheAudio(SELECTOR_SOUND);
    tt::AudioLocator::sound()->cacheAudio(TOMWILLKILL_SOUND);
}

void IntroScreen::draw()
{
    Screen::draw();
    if (_gui) _gui->draw();
}

PollResult IntroScreen::poll(const sf::Event& e)
{
    if (_gui)
    {
        _gui->handleEvent(e);

        if (e.type == sf::Event::KeyReleased
            && e.key.code == sf::Keyboard::Escape)
        {
            _gui.reset();
        }

        return {};
    }

    if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Up)
    {
        if (_selected > 0)
        {
            _selected--;
            updateMenu(_selected, _menuItems);
            tt::AudioLocator::sound()->play(SELECTOR_SOUND);
        }
    }
    else if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Down)
    {
        if (_selected < _menuItems.size() - 1)
        {
            _selected++;
            updateMenu(_selected, _menuItems);

            tt::AudioLocator::sound()->play(SELECTOR_SOUND);
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

            case 0: // new game
            {
                _gui = std::make_unique<tgui::Gui>(_window);

                auto windowSize = _window.getSize();
                auto halfWindowWidth = windowSize.x / 2;

                auto child = tgui::ChildWindow::create();
                child->setClientSize({500, 200});
                auto xpos = (_window.getSize().x / 2) - (child->getSize().x / 2);
                auto ypos = (_window.getSize().y / 2) - (child->getSize().y / 2);
                child->setPosition(xpos, ypos);
                child->setTitleButtons(tgui::ChildWindow::TitleButton::None);
                _gui->add(child);

                auto editLbl = tgui::Label::create("Enter name for new game");
                editLbl->setPosition(20,20);
                editLbl->setTextSize(30);
                child->add(editLbl);

                auto editBox = tgui::EditBox::create();
                editBox->setPosition(20,75);
                editBox->setTextSize(30);
                editBox->setSize(460,35);
                child->add(editBox);

                auto okBtn = tgui::Button::create("Ok");
                okBtn->setWidgetName("sds");
                okBtn->setSize(50, 45);
                xpos = (child->getSize().x / 2) - (okBtn->getSize().x + 20);
                okBtn->setPosition(xpos, 125);
                child->add(okBtn);

                auto cancelBtn = tgui::Button::create("Cancel");
                cancelBtn->setSize(65, 45);
                xpos = (child->getSize().x / 2) + 20;
                cancelBtn->setPosition(xpos, 125);
                cancelBtn->onPress([=]
                    {
                        _gui.reset();
                    });
                child->add(cancelBtn);

                editBox->setFocused(true);

                break;
            }

            case 1: // load game
            {
                return {true, { ScreenActionType::CHANGE_SCREEN, SCREEN_LOADING }};
            }

            case 2: // settings
            {
                return {true, { ScreenActionType::CHANGE_SCREEN, SCREEN_SETTINGS }};
            }

            case 3: // exit
            {
                tt::AudioLocator::music()->stop(BACKGROUND_SONG);
                tt::AudioLocator::sound()->play(TOMWILLKILL_SOUND);

                while (tt::AudioLocator::sound()->getStatus(TOMWILLKILL_SOUND) == sf::Sound::Playing)
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

ScreenAction IntroScreen::update()
{
    static int  alpha           = 0;
    static bool increaseAlpha   = true;
    static int  imageIndex      = 0;

    auto winWidth       = _window.getSize().x;
    auto winHeight      = _window.getSize().y;

    if (auto elapsed = _clock.getElapsedTime();
        elapsed.asMilliseconds() > 150)
    {
        //
        // Fade image in and out.
        //
        if(increaseAlpha) 
        {
            // _sprite[imageIndex]->setScale(
            //    winWidth    / _sprite[imageIndex]->getLocalBounds().width,
            //    winHeight   / _sprite[imageIndex]->getLocalBounds().height );

            alpha += 16;
            if(alpha > 255) 
            {
                increaseAlpha = false;
                alpha = 255;
            }
        } 
        else 
        { 
            alpha -= 16;
            if(alpha < 0) 
            {
                increaseAlpha = true;
                alpha = 0;

                _sprite[imageIndex]->setColor(sf::Color(0, 0, 0, 0));

                //
                // Move to new image.
                //
                imageIndex++;


                if(imageIndex > (_bgt.size() - 1) ) 
                {
                    //
                    // Reset to initial image 
                    //
                    imageIndex = 0;
                }

                auto logger = log::initializeLogger("Intro");
                logger->debug("Change intro image: {}", imageIndex);

                _sprite[imageIndex]->setColor(sf::Color(255, 255, 255, alpha));
            }
        }
        _sprite[imageIndex]->setColor(sf::Color(255, 255, 255, alpha));

        _clock.restart();
    }

    return {};
}

void IntroScreen::close()
{
    Screen::close();
    tt::AudioLocator::music()->stop(BACKGROUND_SONG);
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

    logoText->setOutlineColor(sf::Color(0,0,0,255));
    logoText->setOutlineThickness(5);

    auto txpos = (_window.getSize().x / 2) - ((logoText->getLocalBounds().width) / 2);
    logoText->setPosition(txpos, _window.getSize().y - 150.0f);

    addDrawable(logoText);

    tt::AudioLocator::sound()->cacheAudio(tt::IntroScreen::TOMWILLKILL_SOUND);

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

ScreenAction SplashScreen::update()
{
    if (auto elapsed = _clock.getElapsedTime();
        elapsed.asMilliseconds() > 1500)
    {
        tt::AudioLocator::sound()->stop(tt::IntroScreen::TOMWILLKILL_SOUND);
        return { ScreenActionType::CHANGE_SCREEN, SCREEN_INTRO };
    }

    return {};
}

} // namespace tt
