#pragma once

#include <SFML/Graphics.hpp>

#include "../Background.h"
#include "../Player.h"

#include "Scene.h"

namespace tt
{

class Opening : public Scene
{

public:
    Opening(ResourceManager& resmgr, sf::RenderTarget& target);

    std::uint16_t poll(const sf::Event& e) override;
    std::uint16_t timestep() override;

private:

    void adjustView();

    BackgroundSharedPtr         _background;
    PlayerPtr                   _player;

    std::shared_ptr<sf::Text>   _debugText;

    sf::Clock _globalClock;
};

} // namespace tt