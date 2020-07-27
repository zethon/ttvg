#include <iostream>
#include <ostream>
#include <limits>

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/range/adaptor/indexed.hpp>

#include <SFML/Graphics.hpp>

#include "../src/Vehicle.h"
#include "../src/VehicleFactory.h"
#include "../src/TTUtils.h"
#include "../src/Intersection.h"
#include "../src/PathFactory.h"

#include "Test.h"

using namespace std::string_literals;

namespace data = boost::unit_test::data;

namespace std
{

std::ostream& operator<<(std::ostream& out, const tt::TurningPoint& item)
{
    auto[point, directions, decisionPoint] = item;
    out << "{ point=" << point
        << " turnType=" << item.turn
        << " decisionPoint=" << decisionPoint
        << "}";

    return out;
}

std::ostream& operator<<(std::ostream& out, const tt::TurningPoints& item)
{
    for (const auto& i : item)
    {
        out << i << ' ';
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, const tt::TurningPointParser::Edge& item)
{
    out << '{' << std::get<0>(item) << ',' << static_cast<std::uint32_t>(std::get<1>(item)) << '}';
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

const std::tuple<std::string, std::vector<tt::TurningPoint>> parseIntersectionTestData[] =
{
    {
        "2,2,L0,single,single",
        {
            { sf::Vector2i{3,2}, tt::Direction::UP, false },
            { sf::Vector2i{2,3}, tt::Direction::RIGHT, false },
        }
    }
};

bool intersection_sorter(const tt::TurningPoint& lhs, const tt::TurningPoint& rhs)
{
    if (lhs.point.x != rhs.point.x)
    {
        return lhs.point.x < rhs.point.x;
    }

    return lhs.point.y < rhs.point.y;
}

// --run_test=tt/parse_IntersectionTest
BOOST_DATA_TEST_CASE(parse_IntersectionTest, data::make(parseIntersectionTestData), original, expectedv)
{
    tt::IntersectionParser parser;
    auto result = parser.parse(original.begin(), original.end());
    BOOST_TEST(result.has_value());
    auto[source, type, hzdbl, vtdbl] = *result;
    
    auto iv = tt::makeIntersection(sf::Vector2i{ source }, type, hzdbl, vtdbl);
    BOOST_TEST_REQUIRE(iv.size() == expectedv.size());

    std::sort(iv.begin(), iv.end(), &intersection_sorter);

    // I guess the test's variable is const?
    TurningPoints expected = expectedv;
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

const std::tuple<std::string, tt::TurningPoint> parseTurningPointTestData[] =
{
    {
        "2,2,right",
        tt::TurningPoint{ sf::Vector2i{2,2}, tt::Direction::RIGHT, false }
    },
    {
        "2,2,right,true",
        tt::TurningPoint{ sf::Vector2i{2,2}, tt::Direction::RIGHT, true }
    }
};

// --run_test=tt/parse_turningPointTest
BOOST_DATA_TEST_CASE(parse_turningPointTest, data::make(parseTurningPointTestData), original, expected)
{
    tt::TurningPointParser parser;

    auto start = std::begin(original);
    const auto stop = std::end(original);
    auto result = parser.parse(start, stop);
    BOOST_TEST(result.has_value());

    auto intersection = *result;
    BOOST_TEST(intersection.point == expected.point);
    BOOST_TEST(intersection.turn == expected.turn);
    BOOST_TEST(intersection.decisionPoint == expected.decisionPoint);
}

const std::tuple<sf::Vector2i, tt::IntersectionType, tt::TurningPoints> intersectionTestData[] =
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

// --run_test=tt/intersectionTest
BOOST_DATA_TEST_CASE(intersectionTest, data::make(intersectionTestData), source, type, expectedv)
{
    auto iv = tt::makeIntersection(source, type);
    BOOST_TEST_REQUIRE(iv.size() == expectedv.size());

    std::sort(iv.begin(), iv.end(), &intersection_sorter);

    // I guess the test's variable is const?
    TurningPoints expected = expectedv;
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

const std::tuple<sf::FloatRect, sf::FloatRect, tt::Direction, float, bool> vehicleBlocks[]
{
    // rects the same size
    {
        sf::FloatRect{ 0, 0, 10, 10},
        sf::FloatRect{ 20, 0, 10, 10},
        tt::Direction::RIGHT,
        15.0f,
        true
    },
    {
        sf::FloatRect{ 0, 0, 10, 10},
        sf::FloatRect{ 20, 5, 10, 10},
        tt::Direction::RIGHT,
        15.0f,
        true
    },
    {
        sf::FloatRect{ 0, 5, 10, 10},
        sf::FloatRect{ 20, 0, 10, 10},
        tt::Direction::RIGHT,
        15.0f,
        true
    },

    // `object` is taller
    {
        sf::FloatRect{ 0, 0, 10, 20},
        sf::FloatRect{ 20, 5, 15, 15},
        tt::Direction::RIGHT,
        15.0f,
        true
    },
    {
        sf::FloatRect{ 0, 2, 10, 18},
        sf::FloatRect{ 20, 0, 15, 5},
        tt::Direction::RIGHT,
        15.0f,
        true
    },
    {
        sf::FloatRect{ 0, 0, 10, 10},
        sf::FloatRect{ 20, 8, 15, 4},
        tt::Direction::RIGHT,
        15.0f,
        true
    },
};

// --run_test=tt/vehicleBlockedTest
BOOST_DATA_TEST_CASE(vehicleBlockedTest, data::make(vehicleBlocks), object, other, direction, distance, blocked)
{
    BOOST_TEST(tt::isPathBlocked(object, other, direction, distance) == blocked);
}

// --run_test=tt/path_forcedDoubleLaneTest
BOOST_AUTO_TEST_CASE(path_ForcedSingleLaneTest)
{
    TurningPoints edges =
    {
        { { -1, 2 }, Direction::RIGHT, false },
    };

    TurningPoints inter;

    auto temp = tt::makeIntersection({ 2, 1 }, tt::IntersectionType::L180);
    inter.insert(inter.end(), temp.begin(), temp.end());

    temp = tt::makeIntersection({ 2, 7 }, tt::IntersectionType::L0);
    inter.insert(inter.end(), temp.begin(), temp.end());

    temp = tt::makeIntersection({ 5, 7 }, tt::IntersectionType::L270);
    inter.insert(inter.end(), temp.begin(), temp.end());

    tt::PathFactory fact{ sf::Vector2i{10, 10} };
    fact.setEdges(edges);
    fact.setTurningPoints(inter);

    auto path = fact.makeRiboPath();
    
    std::vector<sf::Vector2f> expected = 
    {
        {-1,2}, {2,2}, {2,8}, {6,8},{6,-1}
    };

    BOOST_TEST(path.points() == expected);
}

// --run_test=tt/path_forcedDoubleLaneTest
BOOST_AUTO_TEST_CASE(path_forcedDoubleLaneTest)
{
    TurningPoints edges =
    {
        { { -1, 3 }, Direction::RIGHT, false },
    };

    TurningPoints inter;

    auto temp = tt::makeIntersection({ 2, 1 }, tt::IntersectionType::L180, tt::LaneSize::DOUBLE, tt::LaneSize::DOUBLE);
    inter.insert(inter.end(), temp.begin(), temp.end());

    temp = tt::makeIntersection({ 2, 7 }, tt::IntersectionType::L0, tt::LaneSize::DOUBLE, tt::LaneSize::DOUBLE);
    inter.insert(inter.end(), temp.begin(), temp.end());

    temp = tt::makeIntersection({ 5, 7 }, tt::IntersectionType::L270, tt::LaneSize::DOUBLE, tt::LaneSize::DOUBLE);
    inter.insert(inter.end(), temp.begin(), temp.end());

    tt::PathFactory fact{ sf::Vector2i{10, 10} };
    fact.setEdges(edges);
    fact.setTurningPoints(inter);

    auto path = fact.makeRiboPath();

    std::vector<sf::Vector2f> expected =
    {
        {-1,3}, {2,3}, {2,9}, {7,9},{7,-1}
    };

    BOOST_TEST(path.points() == expected);
}

// --run_test=tt/pathMakingTXTest
BOOST_AUTO_TEST_CASE(pathMakingTXTest)
{
    TurningPoints edges =
    {
        { { -1, 3 }, Direction::RIGHT, false },
    };

    TurningPoints inter;

    auto temp = tt::makeIntersection({ 2, 2 }, tt::IntersectionType::T90);
    inter.insert(inter.end(), temp.begin(), temp.end());

    tt::PathFactory fact{ sf::Vector2i{10, 10} };
    fact.setEdges(edges);
    fact.setTurningPoints(inter);

    auto path = fact.makeRiboPath();
}

BOOST_AUTO_TEST_CASE(pathMakingSingleTXTest)
{
    TurningPoints edges =
    {
        { { -1, 4 }, Direction::RIGHT, false }
    };

    TurningPoints inter;

    auto temp = tt::makeIntersection({ 3, 3 }, tt::IntersectionType::CROSS);
    inter.insert(inter.end(), temp.begin(), temp.end());

    tt::PathFactory fact{ sf::Vector2i{7, 7} };
    fact.setEdges(edges);
    fact.setTurningPoints(inter);

    auto path = fact.makeRiboPath();
}

// --run_test=tt/pathMakingDoubleTXTest
BOOST_AUTO_TEST_CASE(pathMakingDoubleTXTest)
{
    TurningPoints edges =
    {
        { { -1, 4 }, Direction::RIGHT, false },
        { { -1, 8 }, Direction::RIGHT, false },
    };

    TurningPoints inter;

    auto temp = tt::makeIntersection({ 3, 3 }, tt::IntersectionType::CROSS);
    inter.insert(inter.end(), temp.begin(), temp.end());

    temp = tt::makeIntersection({ 3, 7 }, tt::IntersectionType::CROSS);
    inter.insert(inter.end(), temp.begin(), temp.end());

    tt::PathFactory fact{ sf::Vector2i{11, 11} };
    fact.setEdges(edges);
    fact.setTurningPoints(inter);

    auto path = fact.makeRiboPath();
}

BOOST_AUTO_TEST_CASE(getDirectionTest)
{
    sf::Vector2f startf{ 0.0f, 0.0f };
    sf::Vector2f stopf{ 1.0f, 1.0f };

    auto direction = tt::getDirection(startf, stopf);
    BOOST_TEST(direction & Direction::RIGHT);
    BOOST_TEST(direction & Direction::DOWN);

    direction = tt::getDirection(stopf, startf);
    BOOST_TEST(direction & Direction::LEFT);
    BOOST_TEST(direction & Direction::UP);

    direction = tt::getDirection(sf::Vector2i{ -1, -1 }, sf::Vector2i{ 1,1 });
    BOOST_TEST(direction & Direction::RIGHT);
    BOOST_TEST(direction & Direction::DOWN);

    direction = tt::getDirection(sf::Vector2i{ 0, 10 }, sf::Vector2i{ 0, 100 });
    BOOST_TEST(direction & Direction::DOWN);
    BOOST_TEST(direction ^ Direction::UP);
    BOOST_TEST(direction ^ Direction::LEFT);
    BOOST_TEST(direction ^ Direction::RIGHT);
}

BOOST_AUTO_TEST_CASE(oneBitSetTest)
{
    BOOST_TEST(tt::exactly_one_bit_set(1) == true);
    BOOST_TEST(tt::exactly_one_bit_set(2) == true);
    BOOST_TEST(tt::exactly_one_bit_set(16) == true);

    BOOST_TEST(tt::exactly_one_bit_set(0) == false);
    BOOST_TEST(tt::exactly_one_bit_set(22) == false);
    BOOST_TEST(tt::exactly_one_bit_set(std::numeric_limits<int>::max()) == false);

    auto direction = tt::getDirection(sf::Vector2i{ -1,11 }, sf::Vector2i{ 10,11 });
    BOOST_TEST(tt::exactly_one_bit_set(direction) == true);
}

BOOST_AUTO_TEST_SUITE_END() // tt
