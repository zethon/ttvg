#pragma once

#include <string>
#include <ostream>

#include <SFML/Graphics.hpp>

namespace tt
{

template<typename T>
std::ostream& operator<<(std::ostream& out, const sf::Rect<T> item)
{
    auto [left, top, width, height] = item;
    out << "{ left=" << left
        << " top=" << top
        << " width=" << width
        << " height=" << height
        << "}";

    return out;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const sf::Vector2<T> item)
{
    auto [x,y] = item;
    out << "{ x=" << x
        << " y=" << y
        << "}";

    return out;
}

std::string defaultResourceFolder();
    
} // namespace tt