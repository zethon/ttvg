#pragma once

#include <TGUI/TGUI.hpp>

#include "Screen.h"

namespace tt
{

class SettingsScreen : public Screen
{

public:
    SettingsScreen(ResourceManager& res, sf::RenderTarget& target);

    void draw() override;
    PollResult poll(const sf::Event& e) override;

private:
    sf::Font                    _font;
    std::unique_ptr<tgui::Gui>  _gui;
};

} // namespace tt