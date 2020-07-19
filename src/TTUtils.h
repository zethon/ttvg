#pragma once

#include <string>
#include <ostream>
#include <random>
#include <iterator>
#include <cmath>

#include <boost/spirit/home/x3.hpp>

#include <SFML/Graphics.hpp>

namespace tt
{

std::string defaultResourceFolder();

template<typename T>
std::ostream& operator<<(std::ostream& out, const sf::Rect<T> item)
{
    auto [left, top, width, height] = item;
    out << "{ x=" << left
        << " y=" << top
        << " w=" << width
        << " h=" << height
        << " }";

    return out;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const sf::Vector2<T> item)
{
    auto [x,y] = item;
    out << "{ x=" << x
        << " y=" << y
        << " }";

    return out;
}

//template<typename V>
//sf::Vector2f getTileFromGlobal(const V& position, const sf::Vector2i& tileSize)
//{
//    auto x = static_cast<float>(std::floor(position.x / tileSize.x));
//    auto y = static_cast<float>(std::floor(position.y / tileSize.y));
//    return { x, y };
//}
//
//template<typename Int>
//sf::Vector2f getTileFromGlobal(Int posx, Int posy, const sf::Vector2i& tileSize)
//{
//    return getTileFromGlobal(sf::Vector2<Int>{ posx, posy }, tileSize);
//}

template<typename V>
sf::Vector2f getGlobalFromTile(const V& tilepos, const sf::Vector2i& tileSize)
{
    auto x = static_cast<float>(tilepos.x * tileSize.x);
    auto y = static_cast<float>(tilepos.y * tileSize.y);
    return { x, y };
}

template<typename Int>
sf::Vector2f getGlobalFromTile(Int posx, Int posy, const sf::Vector2i& tileSize)
{
    return getGlobalFromTile(sf::Vector2<Int>{ posx, posy }, tileSize);
}

namespace x3 = boost::spirit::x3;
const auto FloatRectParser
    = x3::rule<class FloatRectParser_, sf::FloatRect>{}
    = (x3::float_ >> ',' >> x3::float_ >> ',' >> x3::float_ >> ',' >> x3::float_)
        [([](auto& ctx)
            {
                auto& attr = x3::_attr(ctx);
                using boost::fusion::at_c;

                auto width = at_c<2>(attr) - at_c<0>(attr);
                auto height = at_c<3>(attr) - at_c<1>(attr);
                x3::_val(ctx)
                    = sf::FloatRect{ at_c<0>(attr), at_c<1>(attr), width, height };
            })
        ];

// custom version of `contains` that will return true if the testing
// point lies on the rectangles border
template<typename Rect, typename T>
bool RectContains(Rect rect, T x, T y)
{
    // Rectangles with negative dimensions are allowed, so we must handle them correctly

    // Compute the real min and max of the rectangle on both axes
    auto minX = std::min(rect.left, static_cast<T>(rect.left + rect.width));
    T maxX = std::max(rect.left, static_cast<T>(rect.left + rect.width));
    T minY = std::min(rect.top, static_cast<T>(rect.top + rect.height));
    T maxY = std::max(rect.top, static_cast<T>(rect.top + rect.height));

    return (x >= minX) && (x <= maxX) && (y >= minY) && (y <= maxY);
}

template <typename T>
bool RectContains(const sf::Rect<T>& rect, const sf::Vector2<T>& point)
{
    return RectContains(rect, point.x, point.y);
}

template<typename T>
auto select_randomly(const T& container)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(0ul, static_cast<int>(std::size(container) - 1));
    auto start = container.begin();
    std::advance(start, dis(gen));
    return start;
}

template <typename T,
    typename = typename std::enable_if<(std::is_integral<T>::value )>::type>
inline bool exactly_one_bit_set(T n)
{
    return n && !(n & (n - 1));
}

    
} // namespace tt