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

    std::uint16_t               _selected = 0;
    TextList                    _menuItems;
    
    std::shared_ptr<sf::Sprite>  _sprite;
    std::unique_ptr<sf::Music>   _bgsong;

    sf::Clock   _clock;

public:
    explicit IntroScreen(ResourceManager& res, sf::RenderTarget& target);

    void poll(const sf::Event& e) override;
    void timestep() override;
};



} // namespace tt