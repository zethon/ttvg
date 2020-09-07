#pragma once

#include "../Screen.h"

namespace tt
{

class DescriptionText : public Screen
{
    sf::Font                            _font;
    std::shared_ptr<sf::Text>           _text;
    std::shared_ptr<sf::RectangleShape> _background;

public:
    static constexpr auto CLASS_NAME = "DescriptionText";
    static const struct luaL_Reg LuaMethods[];

    DescriptionText(ResourceManager& resmgr, sf::RenderTarget& target);

    void setText(const std::string& text);
    std::string text() const;
};

} // namespace