#pragma once

#include "../Screens/Screen.h"

namespace tt
{

class DebugWindow : public Screen
{
    sf::Font    _debugFont;

    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _debugText;

public:
    DebugWindow(ResourceManager& resmgr, sf::RenderTarget& target);

    void setText(const std::string& text);
};

}  // namespace tt