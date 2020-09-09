#pragma once
#include <deque>

#include "../Screen.h"

namespace tt
{

class ModalWindow : public Screen
{

protected:
    sf::Font    _font;

    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _text;

public:
    ModalWindow(ResourceManager& resmgr, sf::RenderTarget& target);
    virtual ~ModalWindow() override = default;

    PollResult poll(const sf::Event& e) override;
    
    void setText(const std::string& text);
};

class MessagesWindow : public ModalWindow
{
    std::deque<std::string>    _messages;

public:
    MessagesWindow(ResourceManager& resmgr, sf::RenderTarget& target);

    void pushMessage(const std::string& message)
    {
        if (_text->getString().getSize() == 0)
        {
            _text->setString(message);
        }

        _messages.push_back(message);
    }

    PollResult poll(const sf::Event& e) override;
    
};

} // namespace tt