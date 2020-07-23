#pragma once
#include <vector>

#include <SFML/Graphics.hpp>

namespace tt
{

class Path
{
public:
    using VectorList = std::vector<sf::Vector2f>;

    bool repeating() const { return _repeating; }
    void setRepeating(bool v) { _repeating = v; }

    bool finished() const 
    { 
        return _finished;
    }

    VectorList& points()
    {
        return const_cast<VectorList&>((static_cast<const Path&>(*this)).points());
    }

    const VectorList& points() const
    {
        return _points;
    }

    [[maybe_unused]] sf::Vector2f step()
    {
        if ((_idx + 1) == _points.size())
        { 
            if (!_repeating)
            {
                _finished = true;
                return _points.back();
            }

            _idx = 0;
            _cycles++;
        }
        else
        {
            _idx++;
        }

        return _points.at(_idx);
    }

    [[maybe_unused]] sf::Vector2f next() const
    {
        auto nextidx = _idx + 1;
        if (_points.size() == nextidx)
        {
            if (_repeating)
            {
                nextidx = 0;
            }
            else
            {
                nextidx = _points.size() - 1;
            }
        }
        return _points.at(nextidx);
    }

    auto current() const
    {
        return _points.at(_idx);
    }

private:

    VectorList      _points;
    std::size_t     _idx = 0;
    std::size_t     _cycles = 0;
    bool            _repeating = true;
    bool            _finished = false;
};

} // namespace tt