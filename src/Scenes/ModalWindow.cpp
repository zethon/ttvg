#include <map>

#include <SFML/Graphics.hpp>

#include <lua/lua.hpp>

#include <fmt/core.h>

#include "../Player.h"
#include "../TTLua.h"
#include "../Scenes/Scene.h"

#include "ModalWindow.h"

namespace tt
{

int Modal_exec(lua_State* L)
{
    auto w = checkModal<ModalWindow>(L);
    w->exec();
    return 0;
}

int Modal_setText(lua_State* L)
{
    auto w = checkModal<ModalWindow>(L);
    const auto text = lua_tostring(L, 2);
    w->setText(text);
    return 0;
}

int Modal_setHeight(lua_State* L)
{
    auto w = checkModal<ModalWindow>(L);
    float height = static_cast<float>(lua_tonumber(L, 2));
    w->setHeight(height);
    return 0;
}

int Modal_setWidth(lua_State* L)
{
    auto w = checkModal<ModalWindow>(L);
    float width = static_cast<float>(lua_tonumber(L, 2));
    w->setWidth(width);
    return 0;
}

int Modal_setAlignment(lua_State* L)
{
    auto w = checkModal<ModalWindow>(L);
    ModalWindow::Alignment mt = static_cast<ModalWindow::Alignment>(lua_tointeger(L, 2));
    w->setAlignment(mt);
    return 0;
}

const struct luaL_Reg ModalWindow::LuaMethods[] =
{
    {"exec", Modal_exec},
    {"setText", Modal_setText},
    {"setHeight", Modal_setHeight},
    {"setWidth", Modal_setWidth},
    {"setAlignment", Modal_setAlignment},
    {nullptr, nullptr}
};

ModalWindow::ModalWindow(Screen& parent)
    : Screen(parent.resources(), parent.window()),
      _parent{ parent }
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

    _border = std::make_shared<sf::RectangleShape>();
    _border->setFillColor(sf::Color::Transparent);
    _border->setOutlineThickness(7.5);
    _border->setOutlineColor(sf::Color::Cyan);

    _background = std::make_shared<sf::RectangleShape>();
    _background->setFillColor(sf::Color{ 0, 0, 0 });

    float width = _window.getSize().x - (_window.getSize().x * 0.35f);
    float height = 175.f;
    _background->setSize(sf::Vector2f{ width, height });

    _text = std::make_shared<sf::Text>("", _font);
    _text->setFillColor(sf::Color::White);
    _text->setCharacterSize(25);

    setAlignment(Alignment::BOTTOM);

    // the order of these calls matters
    addDrawable(_background);
    addDrawable(_border);
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
            
            case sf::Keyboard::Space:
            case sf::Keyboard::Escape:
            {
                return { true, { ScreenActionType::CLOSE_MODAL, {} }};
            }
        }
    }

    return PollResult{};
}

void ModalWindow::setText(const std::string & text)
{
    _text->setString(text);
}

void ModalWindow::setHeight(float height)
{
    _background->setSize(sf::Vector2f{ _background->getSize().x, height });
    setAlignment(_alignment);
}

void ModalWindow::exec()
{
    bool done = false;
    auto window = static_cast<sf::RenderWindow*>(&(_parent.window()));
    while (!done && window->isOpen())
    {
        sf::Event e;
        while (window->pollEvent(e))
        {
            auto result = this->poll(e);
            if (result.handled
                && result.action.type == ScreenActionType::CLOSE_MODAL)
            {
                done = true;
            }
            else if (e.type == sf::Event::EventType::Closed)
            {
                window->close();
            }
                
            window->clear();
            _parent.draw();
            this->draw();
            window->display();
        }
    }
}

void ModalWindow::setWidth(float width)
{
    _background->setSize(sf::Vector2f{ width, _background->getSize().x });
    setAlignment(_alignment);
}

void ModalWindow::setAlignment(ModalWindow::Alignment al)
{
    _alignment = al;
    auto [width, height] = _background->getSize();

    switch (_alignment)
    {
        case ModalWindow::Alignment::TOP:
        {
            float xpos = (_window.getSize().x / 2) - (width / 2);
            float ypos = 25.f;
            _background->setPosition(xpos, ypos);
        }
        break;

        case ModalWindow::Alignment::CENTER:
        {
            float xpos = (_window.getSize().x / 2) - (width / 2);
            float ypos = (_window.getSize().y / 2) - (height / 2);
            _background->setPosition(xpos, ypos);
        }
        break;

        case ModalWindow::Alignment::BOTTOM:
        {
            float xpos = (_window.getSize().x / 2) - (width / 2);
            float ypos = _window.getSize().y - (height + 25.f);
            _background->setPosition(xpos, ypos);
        }
        break;
    }

    auto [xpos, ypos] = _background->getPosition();
    _text->setPosition(xpos + 10, ypos + 5);

    _border->setSize(_background->getSize());
    _border->setPosition(_background->getPosition());
}

////////////////////////////////////////////////////////////////////////////////////////////////

int MessagesWindow_pushMessage(lua_State* L)
{
    auto w = checkObject<MessagesWindow>(L);
    w->pushMessage(lua_tostring(L, 2));
    return 0;
}

const struct luaL_Reg MessagesWindow::LuaMethods[] =
{
    // inherited
    {"exec", Modal_exec},
    {"setText", Modal_setText},
    {"setHeight", Modal_setHeight},
    {"setWidth", Modal_setWidth},
    {"setAlignment", Modal_setAlignment},

    // MessagesWindow
    {"pushMessage", MessagesWindow_pushMessage},
    {nullptr, nullptr}
};

MessagesWindow::MessagesWindow(Screen& parent)
    : ModalWindow{ parent }
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
                    return { true, {} };
                }
                else
                {
                    return { true, { ScreenActionType::CLOSE_MODAL, {} }};
                }
            }
            
            case sf::Keyboard::Escape:
            {
                return { true, { ScreenActionType::CLOSE_MODAL, {} }};
            }
        }
    }

    return PollResult{};
}

//////////////////////////////////////////////////////////////////////////////////////////////

int OptionsWindow_addOption(lua_State* L)
{
    auto w = checkObject<OptionsWindow>(L);
    w->addOption(lua_tostring(L, 2));
    return 0;
}

int OptionsWindow_getSelection(lua_State* L)
{
    auto w = checkObject<OptionsWindow>(L);
    auto sel = w->selection();

    if (sel.has_value())
    {
        lua_pushinteger(L, *sel);
    }
    else
    { 
        lua_pushnil(L);
    }

    return 1;
}

const struct luaL_Reg OptionsWindow::LuaMethods[] =
{
    // inherited
    {"exec", Modal_exec},
    {"setText", Modal_setText},
    {"setHeight", Modal_setHeight},
    {"setWidth", Modal_setWidth},
    {"setAlignment", Modal_setAlignment},

    // OptionsWindow
    {"addOption", OptionsWindow_addOption},
    {"getSelection", OptionsWindow_getSelection},
    {nullptr, nullptr}
};

OptionsWindow::OptionsWindow(Screen& parent)
    : ModalWindow(parent),
      _indicator("->", _font)
{   
    _indicator.setCharacterSize(25);
    _indicator.setFillColor(sf::Color::Yellow);

    auto buffer = _parent.resources().cacheSound("sounds/selector2.wav");
    _selectSound.setBuffer(*buffer);

    buffer = _parent.resources().cacheSound("sounds/selector3.wav");
    _selectionMadeSound.setBuffer(*buffer);
}

PollResult OptionsWindow::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
                return ModalWindow::poll(e);
            
            case sf::Keyboard::Up:
            {
                prevSelection();
                _selectSound.play();
            }
            break;

            case sf::Keyboard::Down:
            {
                nextSelection();
                _selectSound.play();
            }
            break;

            case sf::Keyboard::Escape:
            {
                _selection.reset();
                return { true, { ScreenActionType::CLOSE_MODAL } };
            }                

            case sf::Keyboard::Enter:
            case sf::Keyboard::Space:
            {
                _selectionMadeSound.play();
                while (_selectionMadeSound.getStatus() == sf::SoundSource::Status::Playing)
                {
                    std::this_thread::yield();
                }
                return { true, { ScreenActionType::CLOSE_MODAL }};
            }
        }
    }

    return PollResult{};
}

void OptionsWindow::setText(const std::string& header)
{
    ModalWindow::setText(header);
    adjustLayout();
}

void OptionsWindow::addOption(const std::string& choice)
{
    auto temptext = fmt::format("   {}", choice);
    auto temp = std::make_shared<sf::Text>(temptext, _font);
    temp->setCharacterSize(22);

    _options.emplace_back(std::move(temp));

    addDrawable(_options.back());
    adjustLayout();
}

void OptionsWindow::adjustLayout()
{
    setAlignment(_alignment);

    auto[xanchor, yanchor] = _background->getPosition();
    xanchor += _indicator.getGlobalBounds().width + 15.f;
    yanchor += 5.f;

    _text->setPosition(sf::Vector2f(xanchor, yanchor));
    yanchor += _text->getGlobalBounds().height + 20.f;

    for (auto& choice : _options)
    {
        choice->setPosition(xanchor, yanchor);
        yanchor += choice->getGlobalBounds().height + 7.5f;
    }

    updateText();
}

void OptionsWindow::draw()
{
    ModalWindow::draw();
    _window.draw(_indicator);
}

void OptionsWindow::nextSelection()
{
    if (_selection == (_options.size() - 1))
    {
        _selection = 0;
    }
    else
    {
        assert(_selection.has_value());
        (*_selection)++;
    }

    updateText();
}

void OptionsWindow::prevSelection()
{
    if (_selection == 0)
    {
        _selection = _options.size() - 1;
    }
    else
    {
        (*_selection)--;
    }

    updateText();
}

void OptionsWindow::updateText()
{
    if (_options.size() == 0)
    {
        return;
    }

    std::for_each(_options.begin(), _options.end(),
        [](auto text)
        {
            text->setFillColor(sf::Color::White);
            text->setStyle(sf::Text::Regular);
        });

    assert(_selection.has_value());
    _options.at(*_selection)->setFillColor(sf::Color::Yellow);
    _options.at(*_selection)->setStyle(sf::Text::Style::Bold);

    auto xpos = _text->getPosition().x;
    auto ypos = _options.at(*_selection)->getPosition().y;
    _indicator.setPosition(xpos, ypos);
}

////////////////////////////////////////////////////////////////////////////////////////////////

int Inventory_setDebug(lua_State* L)
{
    auto w = checkObject<InventoryWindow>(L);
    bool d = lua_toboolean(L, 2) == 0 ? false : true;
    w->setDebug(d);
    return 0;
}

int Inventory_getDebug(lua_State* L)
{
    auto w = checkObject<InventoryWindow>(L);
    lua_pushboolean(L, w->debug());
    return 1;
}

const struct luaL_Reg InventoryWindow::LuaMethods[] =
{
    // inherited
    {"exec", Modal_exec},
    {"setText", Modal_setText},
    {"setHeight", Modal_setHeight},
    {"setWidth", Modal_setWidth},
    {"setAlignment", Modal_setAlignment},

    // InventoryWindow specific
    {"setDebug", Inventory_setDebug},
    {"getDebug", Inventory_getDebug},
    {nullptr, nullptr}
};

InventoryWindow::InventoryWindow(Screen& parent, PlayerPtr player, bool debug)
    : OptionsWindow(parent),
      _debug { debug }
{
    setAlignment(ModalWindow::Alignment::CENTER);

    // default text for inventory windo
    setText("You are carrying...");

    for (const auto& item : player->getInventory())
    {
        if (_aggregate.find(item->getID()) != _aggregate.end())
        {
            std::get<0>(_aggregate[item->getID()])++;
        }
        else
        {
            _aggregate[item->getID()] = std::make_tuple(1, item);
        }
    }    

    if (_aggregate.size() >= 10)
    {
        setHeight(height() + 350.f);
    }
    if (_aggregate.size() >= 5)
    {
        setHeight(height() + 175.f);
    }

    updateOptions();
}

void InventoryWindow::updateOptions()
{
    _options.clear();
    for (const auto& info : _aggregate)
    {
        const auto&[count, item] = info.second;
        if (_debug)
        {
            addOption(fmt::format("{} ({}) x {}", item->getName(), item->getID(), count));
        }
        else
        {
            addOption(fmt::format("{} x {}", item->getName(), count));
        }
    }
}

void InventoryWindow::setDebug(bool v) 
{ 
    _debug = v; 
    updateOptions();
}

} // namespace tt