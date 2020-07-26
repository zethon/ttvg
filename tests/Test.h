#pragma once
#include <iostream>

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