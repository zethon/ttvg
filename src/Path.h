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
    bool                        _repeating = true;

public:

    bool repeating() const { return _repeating; }

    std::vector<sf::Vector2i>& points()
    {
        return _points;
    }

    const std::vector<sf::Vector2i>& points() const
    {
        return _points;
    }

    [[maybe_unused]] sf::Vector2i step()
    {
        if (_idx >= _points.size())
        { 
            if (!_repeating) return _points.back();
            _idx = 0;
        }

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