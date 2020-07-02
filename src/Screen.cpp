#include "Screen.h"

namespace tt
{

Screen::Screen(ResourceManager& res, sf::RenderTarget& target)
    : _resources{ res },
      _window{target}
{
}

void Screen::addDrawable(DrawablePtr drawable)
{
    _objects.push_back(drawable);
}

void Screen::clearDrawable()
{
    _objects.clear();
}

void Screen::addUpdateable(IUpdateablePtr updateable)
{
    _updateables.push_back(updateable);
}

void Screen::clearUpdateable()
{
    _updateables.clear();
}

void Screen::draw()
{
    if (!_visible) return;
    
    std::for_each(_objects.begin(), _objects.end(),
        [&](DrawablePtr o) { _window.draw(*o); });
}

std::uint16_t Screen::poll(const sf::Event&)
{
    return 0;
}

std::uint16_t Screen::timestep()
{
    std::for_each(_updateables.begin(), _updateables.end(),
        [](IUpdateablePtr i) { i->timestep(); });

    return 0;
}

} // namespace tt