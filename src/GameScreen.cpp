#include "GameScreen.h"

namespace tt
{

GameScreen::GameScreen(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target)
{
    if (auto temp = _resources.load<sf::Font>("hobo.ttf");
        temp.has_value())
    {
        _font = *temp;
    }
    else
    {
        throw std::runtime_error("hobo.ttf could not be loaded!");
    }

    auto textobj = std::make_shared<sf::Text>("PLAY THE GAME", _font);
    textobj->setPosition(100, 100);
    textobj->setFillColor(sf::Color(255, 1, 129));

    sf::Texture texture = *(_resources.load<sf::Texture>("sprites/tommy.png"));
    texture.setSmooth(true);

    _tt = std::make_shared<AnimatedSprite>(texture, sf::Vector2i{ 64, 64 });
    _tt->setSource(2, 1);
    _tt->setMaxFramesPerRow(9);
    //_tt->setPosition(200, 200);
    _tt->setScale(2.5f, 2.5f);
    _animes.push_back(_tt);

    // load background
    _bgtext = *(_resources.load<sf::Texture>("textures/city1.png"));
    _bgsprite = std::make_shared<sf::Sprite>(_bgtext);


    auto [ttx, tty] = _tt->getPosition();
    tty = _bgtext.getSize().y - ((_tt->getTextureRect().height * 2.5) + 10);
    _tt->setPosition(ttx, tty);
    //_view = _window.getDefaultView();

    auto top1 = _bgtext.getSize().y - _window.getSize().y;
    std::cout << "w: " << _window.getSize().y << '\n';
    std::cout << "b: " << _bgtext.getSize().y << '\n';
    sf::View view(sf::FloatRect(0.f, top1, 1000.f, _window.getSize().y));
    _view = view;
    //_view.zoom(0.75);
    _window.setView(_view);
    
    addDrawable(_bgsprite);
    addDrawable(textobj);
    addDrawable(_tt);
}

std::uint16_t GameScreen::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyReleased
        && e.key.code == sf::Keyboard::Left)
    {
        _tt->setSource(0, 9);
        _tt->setState(AnimatedSprite::State::ANIMATED);
    }

    return 0;
}

std::uint16_t GameScreen::timestep()
{
    std::for_each(_animes.begin(), _animes.end(),
        [](AnimatedSpritePtr a) { a->timestep(); });

    return 0;
}

} // namespace tt