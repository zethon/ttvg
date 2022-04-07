#pragma once

#include <SFML/Audio.hpp>

#include "Screen.h"

#include "../TooterLogger.h"

namespace tt
{

using TextPtr = std::shared_ptr<sf::Text>;
using TextList = std::vector<TextPtr>;

class IntroScreen : public Screen
{
    static const int INTRO_IMAGES = 8;

    sf::Font                    _font;

    sf::Sound                   _tomWillKillSound;
    
    std::shared_ptr<sf::SoundBuffer>    _selectorBuffer;
    std::shared_ptr<sf::SoundBuffer>    _twkBuffer;
    
    std::uint16_t               _selected = 0;
    TextList                    _menuItems;

    std::vector<sf::Texture>    _bgt;

    using SpritePtr             = std::shared_ptr<sf::Sprite>;
    std::vector<SpritePtr>      _sprite;


    std::unique_ptr<sf::Music>   _bgsong;

    sf::Clock   _clock;

public:
    IntroScreen(ResourceManager& res, sf::RenderTarget& target);

    PollResult poll(const sf::Event& e) override;
    ScreenAction update() override;
    void close() override;
};


class SplashScreen : public Screen
{
    sf::Texture     _bg;
    sf::Sound       _tomWillKillSound;
    sf::Clock       _clock;

    sf::Font        _font;

    std::shared_ptr<sf::SoundBuffer>    _twkBuffer;

public:
    SplashScreen(ResourceManager& res, sf::RenderTarget& target);

    PollResult poll(const sf::Event& e) override;
    ScreenAction update() override;
};


} // namespace tt
