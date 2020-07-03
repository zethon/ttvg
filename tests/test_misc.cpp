#include <iostream>
#include <ostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <SFML/Graphics.hpp>

#include "../src/TTUtils.h"

using namespace std::string_literals;

namespace data = boost::unit_test::data;

namespace std
{
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
}

BOOST_AUTO_TEST_SUITE(tt)

const std::tuple<std::string, sf::FloatRect> floatRectParseData[] = 
{
    { "0,0,0,0"s, sf::FloatRect{ 0.f, 0.f, 0.f, 0.f } },
    { "1,2,3,4"s, sf::FloatRect{ 1,2,2,2 } },
    { "1, 2 , 3, 4"s, sf::FloatRect{ 1,2,2,2 } },
    { "1.0, 02.00 , 3   , 004.0"s, sf::FloatRect{ 1,2,2,2 } },
};

// --run_test=tt/testFloatRectParse
BOOST_DATA_TEST_CASE(testFloatRectParse, data::make(floatRectParseData), original, expected)
{
    std::string::const_iterator start = std::begin(original);
    const std::string::const_iterator stop = std::end(original);

    sf::FloatRect rect;
    bool result =
        phrase_parse(start, stop, FloatRectParser, x3::ascii::space, rect);

    BOOST_TEST_REQUIRE(result);
    BOOST_REQUIRE(start == stop);
    BOOST_TEST(rect == expected);
}

BOOST_AUTO_TEST_SUITE_END() // tt
