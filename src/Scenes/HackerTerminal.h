#pragma once

#include "../Screen.h"

namespace tt
{

using TextPtr = std::shared_ptr<sf::Text>;

class HackerTerminal : public Screen
{
    
public:
    HackerTerminal(ResourceManager& resmgr, sf::RenderTarget& target);

private:
    std::unique_ptr<sf::Font>   _monoFont;
    TextPtr                     _soylText;

};

}  // namespace tt
