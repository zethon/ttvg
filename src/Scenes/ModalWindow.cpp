#include "ModalWindow.h"

namespace tt
{

ModalWindow::ModalWindow(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target)
{
    if (auto temp = _resources.load<sf::Font>("fonts/mono_bold.ttf");
        temp.has_value())
    {
        _font = *(temp);
    }
    else
    {
        // TODO: logging?!?!
    }

    _background = std::make_shared<sf::RectangleShape>();

    float width = _window.getSize().x - (_window.getSize().x * 0.2);
    float height = _window.getSize().y - (_window.getSize().y * 0.5);
    _background->setSize(sf::Vector2f{ width, height });

    float xpos = 100.f;
    float ypos = 100.f;
    _background->setPosition(xpos, ypos);
    _background->setFillColor(sf::Color{ 255, 255, 255, 104 });

    _text = std::make_shared<sf::Text>("", _font);
    _text->setFillColor(sf::Color::Red);
    _text->setPosition(xpos, ypos);
    // _text->setPosition(10.f, _window.getSize().y - 35.f);
    _text->setCharacterSize(20);
    
    // the order of these calls matters
    addDrawable(_background);
    addDrawable(_text);
}

PollResult ModalWindow::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;
            
            case sf::Keyboard::Escape:
            {
                return { true, { ScreenActionType::CLOSE_MODAL, {} }};
            }
            break;
        }
    }

    return PollResult{};
}

void ModalWindow::setText(const std::string & text)
{
    _text->setString(text);
}

////////////////////////////////////////////////////////////////////////////////////////////////

MessagesWindow::MessagesWindow(ResourceManager& resmgr, sf::RenderTarget& target)
    : ModalWindow(resmgr, target)
{
}

PollResult MessagesWindow::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;

            case sf::Keyboard::Space:
            {
                _messages.pop_front();
                if (_messages.size() > 0)
                {
                    _text->setString(_messages.front());
                }
                else
                {
                    return { true, { ScreenActionType::CLOSE_MODAL, {} }};
                }
            }
            break;
            
            case sf::Keyboard::Escape:
            {
                return { true, { ScreenActionType::CLOSE_MODAL, {} }};
            }
            break;
        }
    }

    return PollResult{};
}

} // namespace tt