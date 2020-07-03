#pragma once

#include <string>
#include <ostream>

#include <boost/spirit/home/x3.hpp>

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

std::string defaultResourceFolder();
    
} // namespace tt