#include <iostream>
#include <ostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/range/adaptor/indexed.hpp>

#include <SFML/Graphics.hpp>

#include "../src/Vehicle.h"
#include "../src/TTUtils.h"
#include "../src/Intersection.h"

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

template<typename T>
std::ostream& operator<<(std::ostream& out, const sf::Vector2<T> item)
{
    auto[x, y] = item;
    out << "{ x=" << x
        << " y=" << y
        << " }";

    return out;
}

std::ostream& operator<<(std::ostream& out, const tt::Intersection& item)
{
    auto[point, directions, decisionPoint] = item;
    out << "{ point=" << point
        << " turnType=" << item.turn
        << " decisionPoint=" << decisionPoint
        << "}";

    return out;
}

std::ostream& operator<<(std::ostream& out, const tt::Intersections& item)
{
    for (const auto& i : item)
    {
        out << i << ' ';
    }

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

const std::tuple<sf::FloatRect, sf::FloatRect, tt::Vehicle::Direction, float, bool> vehicleBlocks[]
{
    // rects the same size
    {
        sf::FloatRect{ 0, 0, 10, 10},
        sf::FloatRect{ 20, 0, 10, 10},
        tt::Vehicle::RIGHT,
        15.0f,
        true
    },
    {
        sf::FloatRect{ 0, 0, 10, 10},
        sf::FloatRect{ 20, 5, 10, 10},
        tt::Vehicle::RIGHT,
        15.0f,
        true
    },
    {
        sf::FloatRect{ 0, 5, 10, 10},
        sf::FloatRect{ 20, 0, 10, 10},
        tt::Vehicle::RIGHT,
        15.0f,
        true
    },

    // `object` is taller
    {
        sf::FloatRect{ 0, 0, 10, 20},
        sf::FloatRect{ 20, 5, 15, 15},
        tt::Vehicle::RIGHT,
        15.0f,
        true
    },
    {
        sf::FloatRect{ 0, 2, 10, 18},
        sf::FloatRect{ 20, 0, 15, 5},
        tt::Vehicle::RIGHT,
        15.0f,
        true
    },
    {
        sf::FloatRect{ 0, 0, 10, 10},
        sf::FloatRect{ 20, 8, 15, 4},
        tt::Vehicle::RIGHT,
        15.0f,
        true
    },
};

// --run_test=tt/vehicleBlockedTest
BOOST_DATA_TEST_CASE(vehicleBlockedTest, data::make(vehicleBlocks), object, other, direction, distance, blocked)
{
    BOOST_TEST(tt::isPathBlocked(object, other, direction, distance) == blocked);
}

const std::tuple<sf::Vector2i, tt::IntersectionType, tt::Intersections> intersectionTestData[] = 
{
    { 
        sf::Vector2i{ 2,2 }, 
        tt::IntersectionType::L0,
        { 
            { sf::Vector2i{3,2}, tt::Direction::UP, false },
            { sf::Vector2i{2,3}, tt::Direction::RIGHT, false },
        }
    },
    {
        sf::Vector2i{ 2,2 },
        tt::IntersectionType::T0,
        {
            { sf::Vector2i{2,2}, tt::Direction::DOWN, true },
            { sf::Vector2i{3,2}, tt::Direction::LEFT, false },
            { sf::Vector2i{2,3}, tt::Direction::DOWN, true },
            { sf::Vector2i{3,3}, tt::Direction::RIGHT, true },
        }
    }
};


bool intersection_sorter(const tt::Intersection& lhs, const tt::Intersection& rhs) 
{
    if (lhs.point.x != rhs.point.x)
    {
        return lhs.point.x < rhs.point.x;
    }
    
    return lhs.point.y < rhs.point.y;
}

// --run_test=tt/intersectionTest
BOOST_DATA_TEST_CASE(intersectionTest, data::make(intersectionTestData), source, type, expectedv)
{
    auto iv = tt::makeIntersection(source, type);
    BOOST_TEST_REQUIRE(iv.size() == expectedv.size());

    std::sort(iv.begin(), iv.end(), &intersection_sorter);

    // I guess the test's variable is const?
    Intersections expected = expectedv;
    std::sort(expected.begin(), expected.end(), &intersection_sorter);

    BOOST_TEST(iv.size() == expected.size());
    for (const auto& item : (iv | boost::adaptors::indexed()))
    {
        const auto& i = item.value();
        const auto& t = expected.at(item.index());

        BOOST_TEST(i.point == t.point);
        BOOST_TEST(i.decisionPoint == t.decisionPoint);
        BOOST_TEST(i.turn == t.turn);
    }
}

BOOST_AUTO_TEST_SUITE_END() // tt
