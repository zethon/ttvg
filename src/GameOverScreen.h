#pragma once

#include "Screen.h"

namespace tt
{


class GameOverScreen : public Screen
{
    sf::Font    _font;

public:
    GameOverScreen(ResourceManager& res, sf::RenderTarget& target);

    ScreenAction poll(const sf::Event& e) override;
};

} // namespace tt