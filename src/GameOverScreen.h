#pragma once

#include "Screen.h"

namespace tt
{


class GameOverScreen : public Screen
{
    sf::Font    _font;

public:
    GameOverScreen(ResourceManager& res, sf::RenderTarget& target);

    PollResult poll(const sf::Event& e) override;
};

} // namespace tt