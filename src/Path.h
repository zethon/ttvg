#pragma once
#include <vector>

#include <SFML/Graphics.hpp>

namespace tt
{

class Path
{
    std::vector<sf::Vector2i>   _points;
    float                       _speed;
    std::size_t                 _idx = 0;

public:
    std::vector<sf::Vector2i>& points()
    {
        return _points;
    }

    [[maybe_unused]] sf::Vector2i step()
    {
        if (_idx >= _points.size()) _idx = 0;
        return _points.at(_idx++);
    }

    sf::Vector2i next() const
    {
        auto nextidx = _idx;
        if (nextidx >= _points.size()) nextidx = 0;
        return _points.at(nextidx);
    }
};

} // namespace tt