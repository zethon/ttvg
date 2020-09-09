#pragma once
#include <deque>

#include "../Screen.h"

namespace tt
{

class Player;
using PlayerPtr = std::shared_ptr<Player>;

class ModalWindow : public Screen
{

public:
    enum class Alignment
    {
        TOP,
        CENTER,
        BOTTOM
    };

    ModalWindow(ResourceManager& resmgr, sf::RenderTarget& target);
    virtual ~ModalWindow() override = default;

    PollResult poll(const sf::Event& e) override;
    
    void setText(const std::string& text);
    void setAlignment(ModalWindow::Alignment al);

    float width() const { return _background->getSize().x; }
    void setWidth(float width);

    float height() const { return _background->getSize().y; }
    void setHeight(float height);
    
protected:
    sf::Font            _font;
    Alignment           _alignment = Alignment::BOTTOM;

    std::shared_ptr<sf::RectangleShape> _border;
    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _text;
};

////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////

class InventoryWindow : public ModalWindow
{

public:
    InventoryWindow(ResourceManager& resmgr, 
        sf::RenderTarget& target,
        PlayerPtr player);
};

} // namespace tt