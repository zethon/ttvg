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
    ScreenAction update() override;

private:
    void initGui();
    void saveSettings();


    tgui::Label::Ptr            _musicValLbl;
    tgui::Slider::Ptr           _musicSlider;

    tgui::Label::Ptr            _sfxValLbl;
    tgui::Slider::Ptr           _sfxSlider;

    ScreenAction                _result;
    sf::Font                    _font;
    std::unique_ptr<tgui::Gui>  _gui;
};

} // namespace tt