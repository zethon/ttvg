#include <lua/lua.hpp>

#include "DescriptionText.h"

namespace tt
{

[[maybe_unused]] auto checkDescriptionTextObj(lua_State* L)
{
    auto temp = static_cast<DescriptionText**>(luaL_checkudata(L, 1, DescriptionText::CLASS_NAME));
    return *temp;
}

int DescriptionText_getText(lua_State* L)
{
    auto text = checkDescriptionTextObj(L);
    lua_pushstring(L, text->text().c_str());
    return 1;
}

int DescriptionText_setText(lua_State* L)
{
    auto textobj = checkDescriptionTextObj(L);
    auto newtext = lua_tostring(L, 2);
    textobj->setText(newtext);    
    return 0;
}

const struct luaL_Reg DescriptionText::LuaMethods[] =
{
    {"getText", DescriptionText_getText},
    {"setText", DescriptionText_setText},
    {nullptr, nullptr}
};

DescriptionText::DescriptionText(ResourceManager & resmgr, sf::RenderTarget & target)
    : Screen(resmgr, target)
{
    _background = std::make_shared<sf::RectangleShape>();
    _background->setFillColor(sf::Color{ 0, 0, 0, 175 });
    addDrawable(_background);

    if (auto temp = _resources.load<sf::Font>("fonts/mono_bold.ttf");
        temp.has_value())
    {
        _font = *temp;
    }
    else
    {
        // TODO: logging?!?!
    }

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

std::string DescriptionText::text() const
{
    return _text->getString();
}

} // namespace