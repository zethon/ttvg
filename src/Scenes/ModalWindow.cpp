#include "ModalWindow.h"

namespace tt
{

ModalWindow::ModalWindow(ResourceManager & resmgr, sf::RenderTarget & target)
    : Screen(resmgr, target)
{
    setVisible(false);

    if (auto temp = _resources.load<sf::Font>("fonts/mono_bold.ttf");
        temp.has_value())
    {
        _debugFont = *(temp);
        _debugText = std::make_shared<sf::Text>("", _debugFont);
        _debugText->setFillColor(sf::Color::White);
        _debugText->setPosition(10.f, _window.getSize().y - 35.f);
        _debugText->setCharacterSize(20);
        addDrawable(_debugText);
    }
    else
    {
        // TODO: logging?!?!
    }

    _background = std::make_shared<sf::RectangleShape>();
    _background->setFillColor(sf::Color{ 0, 0, 0, 104 });
    _background->setPosition(0.f, _window.getSize().y - 40.f);
    _background->setSize(sf::Vector2f{ static_cast<float>(_window.getSize().x), 40.f });
    addDrawable(_background);
}

void ModalWindow::setText(const std::string & text)
{
    _debugText->setString(text);
}

} // namespace tt