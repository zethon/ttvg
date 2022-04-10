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

    initGui();
}

void SettingsScreen::initGui()
{
    _gui = std::make_unique<tgui::Gui>(_window);

    auto okbutton = tgui::Button::create();
    okbutton->setText("OK");
    okbutton->setSize(100, 30);
    auto okBtnX = (_window.getSize().x / 2) - (okbutton->getSize().x + 10);
    okbutton->setPosition(okBtnX, 550);
    okbutton->onPress([=]
    {
        this->saveSettings();
    });
    _gui->add(okbutton);

    auto cancelbutton = tgui::Button::create();
    cancelbutton->setText("Cancel");
    cancelbutton->setSize(100, 30);
    auto x = (_window.getSize().x / 2) + (okbutton->getSize().x + 10);
    cancelbutton->setPosition(x, 550);
    cancelbutton->onPress([=]
        {
            _result = ScreenAction{ScreenActionType::CHANGE_SCREEN, SCREEN_INTRO};
        });
    _gui->add(cancelbutton);

    auto settings = _resources.settings();

    auto tempValue = settings->value("audio.volume.music", 0u);
    _musicValLbl = tgui::Label::create();
    _musicValLbl->setText(fmt::format("Music {}%", tempValue));
    _musicValLbl->setPosition(okBtnX, 225);
    _musicValLbl->setTextSize(18);
    _musicValLbl->getRenderer()->setTextColor(tgui::Color::White);
    _gui->add(_musicValLbl);

    _musicSlider = tgui::Slider::create();
    _musicSlider->setSize(200,20);
    _musicSlider->setMinimum(0);
    _musicSlider->setMaximum(100);
    _musicSlider->setValue(tempValue);
    _musicSlider->setPosition(_musicValLbl->getPosition().x, 250);
    _musicSlider->onValueChange([=](float value)
        {
            _musicValLbl->setText(fmt::format("Music {}%", value));
        });
    _gui->add(_musicSlider);

    tempValue = settings->value("audio.volume.sfx", 0u);
    _sfxValLbl = tgui::Label::create();
    _sfxValLbl->setText(fmt::format("Sound FX {}%", tempValue));
    x = (_window.getSize().x / 2) - (_sfxValLbl->getSize().x / 2);
    _sfxValLbl->setPosition(okBtnX, 285);
    _sfxValLbl->setTextSize(18);
    _sfxValLbl->getRenderer()->setTextColor(tgui::Color::White);
    _gui->add(_sfxValLbl);

    _sfxSlider = tgui::Slider::create();
    _sfxSlider->setSize(200,20);
    _sfxSlider->setMinimum(0);
    _sfxSlider->setMaximum(100);
    _sfxSlider->setValue(100.f);
    _sfxSlider->setValue(tempValue);
    _sfxSlider->setPosition(_sfxValLbl->getPosition().x, 310);
    _sfxSlider->onValueChange([=](float value)
        {
            _sfxValLbl->setText(fmt::format("Sound FX {}%", value));
        });
    _gui->add(_sfxSlider);
}

void SettingsScreen::draw()
{
    Screen::draw();
    _gui->draw();
}

void SettingsScreen::saveSettings()
{
    auto settings = _resources.settings();

    auto tempValue = static_cast<std::uint32_t>(_musicSlider->getValue());
    settings->set("audio.volume.music", std::to_string(tempValue));

    tempValue = static_cast<std::uint32_t>(_sfxSlider->getValue());
    settings->set("audio.volume.sfx", std::to_string(tempValue));

    _resources.settings()->save();
    _result = ScreenAction{ScreenActionType::CHANGE_SCREEN, SCREEN_INTRO};
}

PollResult SettingsScreen::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Escape)
    {
        return {true, { ScreenActionType::CHANGE_SCREEN, SCREEN_INTRO }};
    }

    _gui->handleEvent(e);

    return {};
}

ScreenAction SettingsScreen::update()
{
    return _result;
}

} // namespace tt