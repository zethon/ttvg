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

void Screen::removeUpdateable(IUpdateablePtr updateable)
{
    _updateables.erase(std::remove_if(_updateables.begin(), _updateables.end(),
        [&updateable](const IUpdateablePtr& o) { return o && (o.get() == updateable.get()); }));
}

void Screen::clearUpdateable()
{
    _updateables.clear();
}

void Screen::draw()
{
    if (!_visible) return;
    
    auto i = _objects.begin();
    while (i != _objects.end())
    {
        auto ptr = *i;
        if (ptr)
        {
            _window.draw(*ptr);
            i++;
        }
        else
        {
            i = _objects.erase(i);
        }
    }
}

ScreenAction Screen::poll(const sf::Event&)
{
    return {};
}

ScreenAction Screen::timestep()
{
    std::for_each(_updateables.begin(), _updateables.end(),
        [](IUpdateablePtr i) { i->timestep(); });

    return {};
}

} // namespace tt