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

ScreenAction EuclidHouse::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;

            case sf::Keyboard::Space:
            {
                return { ScreenActionType::CHANGE_SCENE, 0 };
            }
        }
    }

    return Scene::poll(e);
}

} // namespace tt