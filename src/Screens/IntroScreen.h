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
public:
    inline static constexpr auto BACKGROUND_SONG = "music/intro.wav";
    inline static constexpr auto SELECTOR_SOUND = "sounds/selector.wav";
    inline static constexpr auto TOMWILLKILL_SOUND = "sounds/tomwillkill.wav";

    IntroScreen(ResourceManager& res, sf::RenderTarget& target);

    PollResult poll(const sf::Event& e) override;
    ScreenAction update() override;
    void close() override;

private:
    static const int INTRO_IMAGES = 8;

    sf::Font                    _font;

    std::uint16_t               _selected = 0;
    TextList                    _menuItems;

    std::vector<sf::Texture>    _bgt;

    using SpritePtr             = std::shared_ptr<sf::Sprite>;
    std::vector<SpritePtr>      _sprite;

    sf::Clock   _clock;
};


class SplashScreen : public Screen
{
    sf::Texture     _bg;
    sf::Clock       _clock;
    sf::Font        _font;

public:
    SplashScreen(ResourceManager& res, sf::RenderTarget& target);

    PollResult poll(const sf::Event& e) override;
    ScreenAction update() override;
};


} // namespace tt
