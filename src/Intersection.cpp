#include "Intersection.h"

namespace tt
{

TurningPoints makeIntersection(
    const sf::Vector2i& origin, IntersectionType type, Lane horz, Lane vert)
{
    // note the inversion of horizontal and vertical, this
    // is necessary
    const auto hModifier = (vert == Lane::SINGLE ? 1 : 2);
    const auto vModifier = (horz == Lane::SINGLE ? 1 : 2);

    switch (type)
    {
        default:
        break;

        case IntersectionType::L0:
        {
            return
            { 
                { { origin.x + hModifier, origin.y }, Direction::UP, false },
                { { origin.x, origin.y + vModifier}, Direction::RIGHT, false }
            };
        }
        break;

        case IntersectionType::L90:
        {
            return
            {
                { origin, Direction::DOWN, false },
                { { origin.x + hModifier, origin.y + vModifier}, Direction::RIGHT, false }
            };
        }
        break;

        case IntersectionType::L180:
        {
            return
            {
                { { origin.x + hModifier, origin.y }, Direction::LEFT, false },
                { { origin.x, origin.y + vModifier}, Direction::DOWN, false }
            };
        }
        break;

        case IntersectionType::L270:
        {
            return
            {
                { origin, Direction::LEFT, false },
                { { origin.x + hModifier, origin.y + vModifier}, Direction::UP, false }
            };
        }
        break;

        case IntersectionType::T0:
        {
            return
            {
                { origin, Direction::DOWN, true },
                { { origin.x + hModifier, origin.y}, Direction::LEFT, false },
                { { origin.x, origin.y + vModifier}, Direction::DOWN, true },
                { { origin.x + hModifier, origin.y + vModifier}, Direction::RIGHT, true },
            };
        }

        case IntersectionType::T90:
        {
            return
            {
                { origin, Direction::LEFT, true },
                { { origin.x + hModifier, origin.y}, Direction::LEFT, true },
                { { origin.x, origin.y + vModifier}, Direction::DOWN, true },
                { { origin.x + hModifier, origin.y + vModifier}, Direction::UP, false },
            };
        }

        case IntersectionType::T180:
        {
            return
            {
                { origin, Direction::LEFT, true },
                { { origin.x + hModifier, origin.y}, Direction::UP, true },
                { { origin.x, origin.y + vModifier}, Direction::RIGHT, false },
                { { origin.x + hModifier, origin.y + vModifier}, Direction::UP, true },
            };
        }

        case IntersectionType::T270:
        {
            return
            {
                { origin, Direction::DOWN, false },
                { { origin.x + hModifier, origin.y }, Direction::UP, true },
                { { origin.x, origin.y + vModifier }, Direction::RIGHT, true },
                { { origin.x + hModifier, origin.y + vModifier}, Direction::RIGHT, true },
            };
        }

        case IntersectionType::CROSS:
        {
            return
            {
                { origin, Direction::DOWN | Direction::LEFT, true },
                { { origin.x + hModifier, origin.y }, Direction::UP | Direction::LEFT, true },
                { { origin.x, origin.y + vModifier}, Direction::DOWN | Direction::RIGHT, true },
                { { origin.x + hModifier, origin.y + vModifier}, Direction::UP | Direction::RIGHT, true },
            };
        }
    }

    return {};
}




} // namespace tt