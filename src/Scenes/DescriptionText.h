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
    DescriptionText(ResourceManager& resmgr, sf::RenderTarget& target);

    void setText(const std::string& text);
};

} // namespace