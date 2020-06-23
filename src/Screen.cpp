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
    for (const auto& object : _objects)
    {
        _window.draw(*object);
    }
}

std::uint16_t Screen::poll(const sf::Event&)
{
    return 0;
}

std::uint16_t Screen::timestep()
{
    return 0;
}

} // namespace tt