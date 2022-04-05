#include "SettingsScreen.h"

namespace tt
{

SettingsScreen::SettingsScreen(ResourceManager& res, sf::RenderTarget& target)
    : Screen(res, target)
{
    // I guess I was worried about error handling at one point!
    if (auto temp = _resources.load<sf::Font>("fonts/hobo.ttf");
        temp.has_value())
    {
        _font = *temp;
    }
    else
    {
        throw std::runtime_error("hobo.ttf could not be loaded!");
    }

    auto textobj = std::make_shared<sf::Text>("Settings", _font);
    textobj->setCharacterSize(100);
    textobj->setFillColor(sf::Color::White);

    auto x = (_window.getSize().x / 2) - (textobj->getGlobalBounds().width / 2);
    textobj->setPosition(x, 30);
    addDrawable(textobj);

    _gui = std::make_unique<tgui::Gui>(_window);

    auto label = tgui::Label::create();
    label->setText("Volume");
    x = (_window.getSize().x / 2) - (label->getSize().x / 2);
    label->setPosition(x, 225);
    label->setTextSize(18);
    label->getRenderer()->setTextColor(tgui::Color::White);
    _gui->add(label);

    auto slider = tgui::Slider::create();
    slider->setSize(200,20);
    slider->setValue(100.f);
    slider->setPosition(label->getPosition().x, 250);
    _gui->add(slider);

    auto okbutton = tgui::Button::create();
    okbutton->setText("OK");
    okbutton->setSize(100, 30);
    x = (_window.getSize().x / 2) - (okbutton->getSize().x + 10);
    okbutton->setPosition(x, 550);
    _gui->add(okbutton);

    auto cancelbutton = tgui::Button::create();
    cancelbutton->setText("Cancel");
    cancelbutton->setSize(100, 30);
    x = (_window.getSize().x / 2) + (okbutton->getSize().x + 10);
    cancelbutton->setPosition(x, 550);
    _gui->add(cancelbutton);
}

void SettingsScreen::draw()
{
    Screen::draw();
    _gui->draw();
}

PollResult SettingsScreen::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Space)
    {
        return {true, { ScreenActionType::CHANGE_SCREEN, SCREEN_INTRO }};
    }

    _gui->handleEvent(e);

    return {};
}

} // namespace tt