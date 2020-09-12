#pragma once
#include <deque>

#include "../Screen.h"

namespace tt
{

class Player;
using PlayerPtr = std::shared_ptr<Player>;

enum class ModalType
{
    Default = 0,
    Messages,
    Options,
    Inventory
};

class ModalWindow : public Screen
{

public:
    using CloseHandler = std::function<PollResult(const PollResult&)>;

    enum class Alignment
    {
        TOP,
        CENTER,
        BOTTOM
    };

    ModalWindow(Screen& screen);
    virtual ~ModalWindow() override = default;

    PollResult poll(const sf::Event& e) override;
    
    virtual void setText(const std::string& text);
    void setAlignment(ModalWindow::Alignment al);

    float width() const { return _background->getSize().x; }
    void setWidth(float width);

    float height() const { return _background->getSize().y; }
    void setHeight(float height);

    const CloseHandler& closeHandler() const { return _handler; }
    void setCloseHandler(const CloseHandler&& val) { _handler = val; }

    template<typename T>
    T downcast()
    {
        return static_cast<T>(this);
    }

    void exec();
    
protected:
    sf::Font            _font;
    Alignment           _alignment = Alignment::BOTTOM;
    CloseHandler        _handler;
    Screen&             _parent;

    std::shared_ptr<sf::RectangleShape> _border;
    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _text;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//class MessagesWindow : public ModalWindow
//{
//    std::deque<std::string>    _messages;
//
//public:
//    MessagesWindow(ResourceManager& resmgr, sf::RenderTarget& target);
//
//    void pushMessage(const std::string& message)
//    {
//        if (_text->getString().getSize() == 0)
//        {
//            _text->setString(message);
//        }
//
//        _messages.push_back(message);
//    }
//
//    PollResult poll(const sf::Event& e) override;
//    
//};
//
////////////////////////////////////////////////////////////////////////////////////////////////

class OptionsWindow : public ModalWindow
{

public:
    using TextPtr = std::shared_ptr<sf::Text>;
    using Options = std::vector<TextPtr>;

    OptionsWindow(Screen& parent);

    PollResult poll(const sf::Event& e) override;

    void setText(const std::string& header) override;
    void addOption(const std::string& choice);

    std::optional<std::size_t> selection() const { return _selection; }

private:
    void adjustLayout();
    void draw() override;

    void nextSelection();
    void prevSelection();
    void updateText();

    Options     _options;
    sf::Text    _indicator;
    
    std::optional<std::size_t>  _selection = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

class InventoryWindow : public OptionsWindow
{

public:
    InventoryWindow(Screen& parent, PlayerPtr player);
};

} // namespace tt