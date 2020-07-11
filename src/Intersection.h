#pragma once
#include <string>

#include <SFML/Graphics.hpp>

#include "TTUtils.h"

using namespace std::string_literals;

// Types of L and T intersections
//
//  *            ***         
//  *      LO     *     TO   
// ****           *          
                          
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

constexpr auto MAX_PATH_POINTS = 50;

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

struct Intersection
{
    sf::Vector2i    point;
    std::uint32_t   turn;
    bool            decisionPoint = false;
};

using Intersections = std::vector<Intersection>;

Intersections makeIntersection(const sf::Vector2i& origin, IntersectionType type);

} // namespace tt