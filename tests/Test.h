#pragma once
#include <iostream>

#include <test-config.h>

namespace std
{

template<typename T>
std::ostream& operator<<(std::ostream& out, const sf::Vector2<T> item)
{
    auto[x, y] = item;
    out << "{ x=" << x
        << " y=" << y
        << " }";

    return out;
}

std::ostream& operator<<(std::ostream& out, const sf::FloatRect& item)
{
    auto [left, top, width, height] = item;
    out << "{ left=" << left
        << " top=" << top
        << " width=" << width
        << " height=" << height
        << "}";

    return out;
}

} // namespace std

namespace tt
{

class NullWindow : public sf::RenderTarget
{
public:
    sf::Vector2u getSize() const override
    {
        return sf::Vector2u{4096,4096};
    }
};

}