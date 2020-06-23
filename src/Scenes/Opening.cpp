#include "Opening.h"

namespace tt
{
    
Opening::Opening(ResourceManager& resmgr, sf::RenderTarget& target)
    : Scene(resmgr, target)
{
    sf::Texture temptext = *(_resources.load<sf::Texture>("textures/city1.png"));
    _background = std::make_shared<Background>(temptext);
    _background->setScale(0.4f, 0.4f);
    sf::View view;
    view.setSize(500.f, 350.f);
    _window.setView(view);
    
    temptext = *(_resources.load<sf::Texture>("textures/tommy.png"));
    _player = std::make_shared<AnimatedSprite>(temptext, sf::Vector2i{ 64, 64 });
    _player->texture().setSmooth(true);
    _player->setSource(0,2);
    _player->setScale(2.5f, 2.5f);
    addUpdateable(_player);    

    // the order in which we add everything to the draw'able
    // vector is important, so we do it all at the end of
    // the function
    addDrawable(_background);
    addDrawable(_player);
}

std::uint16_t Opening::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyReleased)
    {
        switch (e.key.code)
        {
            default:
            break;

            case sf::Keyboard::Left:
            {
                sf::View view = _window.getView();
                auto [x,y] = view.getCenter();
                x -= 20;
                view.setCenter(x, y);
                _window.setView(view);
            }
            break;

            case sf::Keyboard::Right:
            {
                sf::View view = _window.getView();
                auto [x,y] = view.getCenter();
                x += 20;
                view.setCenter(x, y);
                _window.setView(view);
            }
            break;

            case sf::Keyboard::Up:
            {
                sf::View view = _window.getView();
                auto [x,y] = view.getCenter();
                y -= 20;
                view.setCenter(x, y);
                _window.setView(view);
            }
            break;

            case sf::Keyboard::Down:
            {
                sf::View view = _window.getView();
                auto [x,y] = view.getCenter();
                y += 20;
                view.setCenter(x, y);
                _window.setView(view);
            }
            break;
        }
    }

    return 0;
}

} // namespace tt