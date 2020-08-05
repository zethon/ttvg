#pragma once

#include <boost/range/adaptor/indexed.hpp>

#include <nlohmann/json.hpp>

#include <SFML/Graphics.hpp>

#include "../Item.h"
#include "../ItemFactory.h"

#include "../Background.h"
#include "../Player.h"

#include "Scene.h"

namespace nl = nlohmann;

namespace tt
{

class EuclidHouse : public Scene
{

public:
    EuclidHouse(ResourceManager& resmgr, sf::RenderTarget& target, PlayerPtr player);

    ScreenAction poll(const sf::Event& e) override;
    void draw() override;

    void enter() override;

private:
    BackgroundSharedPtr                 _background;
};

} // namespace tt
