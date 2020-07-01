#include "Scene.h"

namespace tt
{

Scene::Scene(ResourceManager& res, sf::RenderTarget& target)
    : Screen(res, target)
{
    _debugFont = *(_resources.load<sf::Font>("fonts/mono_bold.ttf"));
}

} // namespace tt