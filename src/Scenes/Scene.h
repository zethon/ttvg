#pragma once

#include "../Screen.h"

namespace tt
{

class Scene;
using ScenePtr = std::unique_ptr<Scene>;
using SceneSharedPtr = std::shared_ptr<Scene>;

class Scene : public Screen
{
    sf::Font    _debugFont;

public:
    Scene(ResourceManager& res, sf::RenderTarget& target);

    sf::Font& debugFont() { return _debugFont; }
};

} // namespace tt