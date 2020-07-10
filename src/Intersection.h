#pragma once
#include <string>

#include <SFML/Graphics.hpp>

#include "TTUtils.h"

using namespace std::string_literals;

namespace tt
{

constexpr auto MAX_PATH_POINTS = 50;

enum class Direction
{
    UP = 0,
    DOWN,
    LEFT,
    RIGHT
};

enum IntersectionType
{
    STRAIGHT,
    L0,
    L90,
    L180,
    L270,
    T0,
    T90,
    T180,
    T270
};

struct Intersection
{
    sf::Vector2i    point;
    std::bitset<4>  directions;
    bool            decisionPoint = false;
};

using Intersections = std::vector<Intersection>;

Intersections generateIntersections(const sf::Vector2i& origin, IntersectionType type, bool decisionPoint = false)
{
    switch (type)
    {
        default:
        break;

        case IntersectionType::L0:
        {
            return { { origin, std::bitset<4>{"1001"}, decisionPoint } };
        }
        break;

        case IntersectionType::L90:
        {
            return { { origin, std::bitset<4>{"0101"}, decisionPoint } };
        }
        break;

        case IntersectionType::L180:
        {
            return { { origin, std::bitset<4>{"0110"}, decisionPoint } };
        }
        break;

        case IntersectionType::L270:
        {
            return { { origin, std::bitset<4>{"1010"}, decisionPoint } };
        }
        break;

        case IntersectionType::T0:
        {
            auto retval = generateIntersections(origin, IntersectionType::L90);

            auto temp = origin;
            temp.x++;
            retval.push_back(generateIntersections(temp, IntersectionType::L180).front());

            temp.y++;
            retval.push_back(generateIntersections(temp, IntersectionType::L90, true).front());

            temp.x--;
            retval.push_back(generateIntersections(temp, IntersectionType::L180, true).front());

            return retval;
        }
    }

    return {};
}



//std::vector<Direction> getPossibleIntersections(Type type, Direction source)
//{
//    switch (type)
//    {
//        case L0:
//        {
//
//        }
//        break;
//    }
//}
//
//////////////////////////////////////////////////////////////
///// \brief A class for all the different types of road intersections
/////
//////////////////////////////////////////////////////////////
//class Intersection
//{
//
//public:
//    enum Type
//    {
//        L0,
//        L90,
//        L180,
//        L270,
//        T0,
//        T90,
//        T180,
//        T270
//    };
//
//    // anchor is the top most and left most tile of the intersection
//    Intersection(Type type)
//    {
//
//    }
//
//    std::vector<Direction> getPossibleDirections(Direction sourceDirection) const
//    {
//        return {};
//    }
//
//    Direction getRandomTurn(Direction sourceDirection) const
//    {
//        const auto directions = getPossibleDirections(sourceDirection);
//        return *(select_randomly(directions));
//    }
//
//private:
//    Type                        _type;
//    std::vector<Direction>      _directions;
//};

} // namespace tt