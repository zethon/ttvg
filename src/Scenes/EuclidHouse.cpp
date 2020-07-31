#pragma once

#include "EuclidHouse.h"

namespace nl = nlohmann;

namespace tt
{
    
EuclidHouse::EuclidHouse(ResourceManager& resmgr, sf::RenderTarget& target)
    : Scene(resmgr, target)
{
    _background = std::make_shared<Background>("EuclidHouse", _resources, sf::Vector2f{ 64, 64 });
    //_background->setScale(SCALE_BACKGROUND, SCALE_BACKGROUND);
    _background->setPosition(0.0f, 0.0f);

    addDrawable(_background);
}

} // namespace tt
