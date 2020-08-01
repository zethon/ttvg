#pragma once

#include <nlohmann/json.hpp>

#include "../Screen.h"

namespace tt
{

class Scene;
using ScenePtr = std::unique_ptr<Scene>;
using SceneSharedPtr = std::shared_ptr<Scene>;

class Scene : public Screen
{

public:
    Scene(ResourceManager& res, sf::RenderTarget& target);

};

} // namespace tt