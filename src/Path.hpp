#pragma once
#include <vector>

#include <SFML/Graphics.hpp>

namespace tt
{

class Path
{
public:
    using VectorList = std::vector<sf::Vector2i>;

    bool repeating() const { return _repeating; }
    void setRepeating(bool v) { _repeating = v; }

    bool finished() const 
    { 
        if (_repeating) return true;
        return (_points.size() - 1) == _idx;
    }

    VectorList& points()
    {
        return const_cast<VectorList&>((static_cast<const Path&>(*this)).points());
    }

    const VectorList& points() const
    {
        return _points;
    }

    [[maybe_unused]] sf::Vector2i step()
    {
        if ((_idx + 1) == _points.size())
        { 
            if (!_repeating)
            {
                return _points.back();
            }

            _idx = 0;
            _cycles++;
        }

        return _points.at(_idx);
    }

    sf::Vector2i next() const
    {
        auto nextidx = _idx + 1;
        if (_points.size() == nextidx)
        {
            nextidx = 0;
        }
        return _points.at(nextidx);
    }

    auto current() const
    {
        return _points.at(_idx);
    }

private:

    VectorList      _points;
    float           _speed;
    std::size_t     _idx = 0;
    std::size_t     _cycles = 0;
    bool            _repeating = true;
};

} // namespace tt