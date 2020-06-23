#include "Opening.h"

namespace tt
{

constexpr auto SCALE_PLAYER = 2.0f;
constexpr auto SCALE_BACKGROUND = 0.7f;
    
Opening::Opening(ResourceManager& resmgr, sf::RenderTarget& target)
    : Scene(resmgr, target)
{
    sf::Texture temptext = *(_resources.load<sf::Texture>("textures/city1.png"));
    _background = std::make_shared<Background>(temptext);
    _background->setScale(SCALE_BACKGROUND, SCALE_BACKGROUND);

    auto top = (_background->texture().getSize().y * 0.7f) - _window.getSize().y;
    sf::View view(sf::FloatRect(0.f, top, 
        static_cast<float>(_window.getSize().x), static_cast<float>(_window.getSize().y)));
    _window.setView(view);
    
    temptext = *(_resources.load<sf::Texture>("textures/tommy.png"));
    _player = std::make_shared<AnimatedSprite>(temptext, sf::Vector2i{ 64, 64 });
    _player->texture().setSmooth(true);
    _player->setSource(0,2);
    _player->setScale(SCALE_PLAYER, SCALE_PLAYER);
    auto [playerx, playery] = _player->getPosition();
    playerx = (_window.getSize().x / 2) - static_cast<float>(_player->getTextureRect().width);
    playery = (_window.getSize().y - (_player->getTextureRect().height * SCALE_PLAYER)) - 5;
    _player->setPosition(playerx, playery);
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