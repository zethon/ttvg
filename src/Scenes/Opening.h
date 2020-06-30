#pragma once

#include <SFML/Graphics.hpp>
#include <tmxlite/Map.hpp>

#include "../Background.h"
#include "../AnimatedSprite.h"
#include "../SFMLOrthogonalLayer.hpp"

#include "Scene.h"

namespace tt
{

using MapPtr = std::shared_ptr<tmx::Map>;

class Opening : public Scene
{

public:
    Opening(ResourceManager& resmgr, sf::RenderTarget& target);

    std::uint16_t poll(const sf::Event& e) override;
    std::uint16_t timestep() override;

private:

    void adjustView();

    BackgroundSharedPtr         _background;
    AnimatedSpritePtr           _player;
    std::shared_ptr<sf::Text>   _debugText;

    std::shared_ptr<tmx::Map> _map;
    std::shared_ptr<MapLayer> _l1;
    std::shared_ptr<MapLayer> _l2;
    std::shared_ptr<MapLayer> _l3;
    sf::Clock _globalClock;
};

} // namespace tt