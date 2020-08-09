#include "DescriptionText.h"

namespace tt
{

DescriptionText::DescriptionText(ResourceManager & resmgr, sf::RenderTarget & target)
    : Screen(resmgr, target)
{
    _background = std::make_shared<sf::RectangleShape>();
    _background->setFillColor(sf::Color{ 0, 0, 0, 175 });
    addDrawable(_background);

    _font = *(_resources.load<sf::Font>("fonts/mono_bold.ttf"));
    _text = std::make_shared<sf::Text>("", _font);
    _text->setFillColor(sf::Color::Yellow);
    _text->setCharacterSize(25);
    addDrawable(_text);
}

void DescriptionText::setText(const std::string & text)
{
    if (text.size() == 0)
    {
        setVisible(false);
        return;
    }

    setVisible(true);
    _text->setString(text);

    auto rect = _text->getGlobalBounds();
    _text->setPosition((_window.getSize().x / 2) - (rect.width / 2), 42.5f);

    auto[x, y, w, h] = _text->getGlobalBounds();

    _background->setSize(sf::Vector2f{ w + 20.f, h + 10.f });
    _background->setPosition(x - 10.f, y - 5.f);
}

} // namespace