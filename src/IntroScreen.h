#pragma once

#include <SFML/Audio.hpp>

#include "Screen.h"

namespace tt
{

using TextPtr = std::shared_ptr<sf::Text>;
using TextList = std::vector<TextPtr>;

class IntroScreen : public Screen
{
    sf::Font    _font;
    sf::Texture _bgt;

    std::shared_ptr<sf::SoundBuffer>    _selectorBuffer;
    std::shared_ptr<sf::SoundBuffer>    _twkBuffer;
    sf::Sound                           _sound;

    std::uint16_t               _selected = 0;
    TextList                    _menuItems;
    
    std::shared_ptr<sf::Sprite>  _sprite;
    std::unique_ptr<sf::Music>   _bgsong;

    sf::Clock   _clock;

public:
    explicit IntroScreen(ResourceManager& res, sf::RenderTarget& target);

    std::uint16_t poll(const sf::Event& e) override;
    std::uint16_t timestep() override;
};



} // namespace tt