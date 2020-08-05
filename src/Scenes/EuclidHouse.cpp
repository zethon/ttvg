#include "EuclidHouse.h"

namespace nl = nlohmann;

namespace tt
{
    
EuclidHouse::EuclidHouse(ResourceManager& resmgr, sf::RenderTarget& target, PlayerPtr player)
    : Scene(resmgr, target, player)
{
    _background = std::make_shared<Background>("EuclidHouse", _resources, sf::Vector2f{ 64, 64 });
    _background->setPosition(0.0f, 0.0f);

    const sf::Vector2f bgscale{
        static_cast<float>(_window.getSize().x) / static_cast<float>(_background->getTexture()->getSize().x),
        static_cast<float>(_window.getSize().y) / static_cast<float>(_background->getTexture()->getSize().y) };
    _background->setScale(bgscale);

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
