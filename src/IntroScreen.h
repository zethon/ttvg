#pragma once

#include "Screen.h"

namespace tt
{

class IntroScreen : public Screen
{
    sf::Font    _font;
    sf::Texture _bgt;
    std::shared_ptr<sf::Sprite>  _sprite;

    sf::Clock   _clock;

public:
    explicit IntroScreen(ResourceManager& res, sf::RenderTarget& target);

    void poll() override;
};



} // namespace tt