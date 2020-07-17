#pragma once
#include <string>
#include <optional>

#include <boost/spirit/home/x3.hpp>

#include <SFML/Graphics.hpp>

#include "TTUtils.h"

using namespace std::string_literals;

// Types of L and T intersections
//
// *             ***         
// *      LO      *     TO   
// ***            *          
                          
//  ***            *         
//  *      L90   ***    T90  
//  *              *         
                          
//  ***           *          
//    *    L180   *     T180 
//    *          ***         
                          
//    *          *           
//    *    L270  ***    T270 
//  ***          *           
                          

namespace tt
{

enum Direction
{
    UP      = 0x01,
    DOWN    = 0x02,
    LEFT    = 0x04,
    RIGHT   = 0x08
};

enum IntersectionType
{
    L0,
    L90,
    L180,
    L270,
    T0,
    T90,
    T180,
    T270, 
    CROSS
};

enum LaneSize
{
    SINGLE,
    DOUBLE
};

struct TurningPoint
{
    sf::Vector2i    point;
    std::uint32_t   turn;
    bool            decisionPoint = false;
};

using TurningPoints = std::vector<TurningPoint>;

TurningPoints makeIntersection(
    const sf::Vector2i& origin, 
    IntersectionType type, 
    LaneSize h = LaneSize::SINGLE, 
    LaneSize v = LaneSize::SINGLE);

namespace x3 = boost::spirit::x3;

struct IntersectionParser
{
    struct IntersectionType_ : x3::symbols<IntersectionType>
    {
        IntersectionType_()
        {
            add
            ("L0", IntersectionType::L0)
                ("L90", IntersectionType::L90)
                ("L180", IntersectionType::L180)
                ("L270", IntersectionType::L270)
                ("T0", IntersectionType::T0)
                ("T90", IntersectionType::T90)
                ("T180", IntersectionType::T180)
                ("T270", IntersectionType::T270)
                ("CROSS", IntersectionType::CROSS)
                ;
        }
    };

    struct Lane_ : x3::symbols<LaneSize>
    {
        Lane_()
        {
            add
                ("single", LaneSize::SINGLE)
                ("double", LaneSize::DOUBLE)
                ;
        }
    };

    using IntersectionHelper
        = std::tuple<sf::Vector2f, tt::IntersectionType, LaneSize, LaneSize>;

    template<typename It>
    std::optional<IntersectionHelper> parse(It begin, It end)
    {
        static auto parser
            = x3::rule<class IntersectionParser_, IntersectionHelper>{}
            = (x3::float_ >> ',' >> x3::float_ >> ',' >> intersectionType_ >> ',' >> laneType_ >> ',' >> laneType_)
            [(
                [](auto& ctx)
                {
                    auto& attr = x3::_attr(ctx);
                    using boost::fusion::at_c;

                    sf::Vector2f pt{ static_cast<float>(at_c<0>(attr)), static_cast<float>(at_c<1>(attr)) };
                    x3::_val(ctx)
                        = IntersectionHelper{ pt, at_c<2>(attr), at_c<3>(attr), at_c<4>(attr) };
                }
        )];

        IntersectionHelper helper;
        bool result = phrase_parse(begin, end, parser, x3::ascii::space, helper);
        if (!result) return {};
        return helper;
    }

private:
    IntersectionType_ intersectionType_;
    Lane_ laneType_;
};

struct TurningPointParser
{
    struct Direction_ : x3::symbols<Direction>
    {
        Direction_()
        {
            add
                ("up", Direction::UP)
                ("down", Direction::DOWN)
                ("left", Direction::LEFT)
                ("right", Direction::RIGHT)
            ;
        }
    };

    using Edge
        = std::tuple<sf::Vector2f, std::uint32_t, bool>;

    template<typename It>
    std::optional<TurningPoint> parse(It begin, It end)
    {
        static auto parser
            = x3::rule<class EdgeParser_, Edge>{}
            = (x3::float_ >> ',' >> x3::float_ >> ',' >> directionType_ >> -(',' >> x3::bool_))
            [(
                [](auto& ctx)
                {
                    auto& attr = x3::_attr(ctx);
                    using boost::fusion::at_c;

                    sf::Vector2f pt{ static_cast<float>(at_c<0>(attr)), static_cast<float>(at_c<1>(attr)) };
                    x3::_val(ctx)
                        = Edge{ pt, 
                            static_cast<std::uint32_t>(at_c<2>(attr)),  
                            at_c<3>(attr) ? *(at_c<3>(attr)) : false };
                }
            )];

        Edge helper;
        bool result = phrase_parse(begin, end, parser, x3::ascii::space, helper);
        if (!result) return {};

        auto[origin, direction, dp] = helper;
        return TurningPoint{ sf::Vector2i{origin}, direction, dp };
    }

private:
    Direction_                          directionType_;
    x3::rule<class EdgeParser_, Edge>   parser_;
};

} // namespace tt