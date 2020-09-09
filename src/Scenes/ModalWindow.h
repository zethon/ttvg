#pragma once

#include "../Screen.h"

namespace tt
{

class ModalWindow : public Screen
{
    sf::Font    _debugFont;

    std::shared_ptr<sf::RectangleShape> _background;
    std::shared_ptr<sf::Text>           _debugText;

public:
    ModalWindow(ResourceManager& resmgr, sf::RenderTarget& target);

    void setText(const std::string& text);
};

} // namespace tt