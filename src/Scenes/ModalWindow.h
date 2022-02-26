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
    enum class Alignment
    {
        TOP,
        CENTER,
        BOTTOM
    };

    static constexpr auto CLASS_NAME = "ModalWindow";
    static const struct luaL_Reg LuaMethods[];

    ModalWindow(Screen& screen);
    virtual ~ModalWindow() override = default;

    PollResult poll(const sf::Event& e) override;
    
    virtual void setText(const std::string& text);
    void setAlignment(ModalWindow::Alignment al);

    float width() const { return _background->getSize().x; }
    void setWidth(float width);

    float height() const { return _background->getSize().y; }
    void setHeight(float height);

    template<typename T>
    T downcast()
    {
        return static_cast<T>(this);
    }

    void exec();
    
protected:
    sf::Font            _font;
    Alignment           _alignment = Alignment::BOTTOM;
    Screen&             _parent;

    std::shared_ptr<sf::RectangleShape> _border;
    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _text;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class MessagesWindow : public ModalWindow
{
    std::deque<std::string>    _messages;

public:
    static constexpr auto CLASS_NAME = "MessagesWindow";
    static const struct luaL_Reg LuaMethods[];

    MessagesWindow(Screen& parent);

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

//////////////////////////////////////////////////////////////////////////////////////////////

class OptionsWindow : public ModalWindow
{

public:
    using TextPtr = std::shared_ptr<sf::Text>;
    using Options = std::vector<TextPtr>;

    static constexpr auto CLASS_NAME = "OptionsWindow";
    static const struct luaL_Reg LuaMethods[];

    OptionsWindow(Screen& parent);

    PollResult poll(const sf::Event& e) override;

    void setText(const std::string& header) override;
    void addOption(const std::string& choice);

    std::optional<std::size_t> selection() const { return _selection; }

protected:
    Options     _options;

private:
    void adjustLayout();
    void draw() override;

    void nextSelection();
    void prevSelection();
    void updateText();

    sf::Text    _indicator;

    std::optional<std::size_t>  _selection = 0;
    sf::Sound                   _selectSound;
    sf::Sound                   _selectionMadeSound;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

class InventoryWindow : public OptionsWindow
{
    bool _debug = false;

    // framecount and name
    using InvAgg = std::tuple<std::uint32_t, ItemPtr>;
    std::map<std::string, InvAgg> _aggregate;

    void updateOptions();

public:
    static constexpr auto CLASS_NAME = "InventoryWindow";
    static const struct luaL_Reg LuaMethods[];

    InventoryWindow(Screen& parent, PlayerPtr player)
        : InventoryWindow(parent, player, false)
    {}
    
    InventoryWindow(Screen& parent, PlayerPtr player, bool);

    void setDebug(bool v);
    bool debug() const { return _debug; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename WinT>
WinT* checkModal(lua_State* L)
{
    if (luaL_testudata(L, 1, ModalWindow::CLASS_NAME))
    {
        auto temp = static_cast<WinT**>(luaL_checkudata(L, 1, ModalWindow::CLASS_NAME));
        return dynamic_cast<WinT*>(*temp);
    }
    else if (luaL_testudata(L, 1, MessagesWindow::CLASS_NAME))
    {
        auto temp = static_cast<WinT**>(luaL_checkudata(L, 1, MessagesWindow::CLASS_NAME));
        return dynamic_cast<WinT*>(*temp);
    }
    else if (luaL_testudata(L, 1, OptionsWindow::CLASS_NAME))
    {
        auto temp = static_cast<WinT**>(luaL_checkudata(L, 1, OptionsWindow::CLASS_NAME));
        return dynamic_cast<WinT*>(*temp);
    }
    else if (luaL_testudata(L, 1, InventoryWindow::CLASS_NAME))
    {
        auto temp = static_cast<WinT**>(luaL_checkudata(L, 1, InventoryWindow::CLASS_NAME));
        return dynamic_cast<WinT*>(*temp);
    }

    return nullptr;
}

} // namespace tt