#include "Intersection.h"

namespace tt
{

Intersections makeIntersection(const sf::Vector2i & origin, IntersectionType type)
{
    switch (type)
    {
        default:
        break;

        case IntersectionType::L0:
        {
            return
            { 
                { { origin.x + 1, origin.y }, Direction::UP, false },
                { { origin.x, origin.y + 1}, Direction::RIGHT, false }
            };
        }
        break;

        case IntersectionType::L90:
        {
            return
            {
                { origin, Direction::DOWN, false },
                { { origin.x + 1, origin.y + 1}, Direction::RIGHT, false }
            };
        }
        break;

        case IntersectionType::L180:
        {
            return
            {
                { { origin.x + 1, origin.y }, Direction::LEFT, false },
                { { origin.x, origin.y + 1}, Direction::DOWN, false }
            };
        }
        break;

        case IntersectionType::L270:
        {
            return
            {
                { origin, Direction::LEFT, false },
                { { origin.x + 1, origin.y + 1}, Direction::UP, false }
            };
        }
        break;

        case IntersectionType::T0:
        {
            return
            {
                { origin, Direction::DOWN, true },
                { { origin.x + 1, origin.y}, Direction::LEFT, false },
                { { origin.x, origin.y + 1}, Direction::DOWN, true },
                { { origin.x + 1, origin.y + 1}, Direction::RIGHT, true },
            };
        }

        case IntersectionType::T90:
        {
            return
            {
                { origin, Direction::LEFT, true },
                { { origin.x + 1, origin.y}, Direction::LEFT, true },
                { { origin.x, origin.y + 1}, Direction::DOWN, true },
                { { origin.x + 1, origin.y + 1}, Direction::UP, false },
            };
        }

        case IntersectionType::T180:
        {
            return
            {
                { origin, Direction::LEFT, true },
                { { origin.x + 1, origin.y}, Direction::UP, true },
                { { origin.x, origin.y + 1}, Direction::RIGHT, false },
                { { origin.x + 1, origin.y + 1}, Direction::UP, true },
            };
        }

        case IntersectionType::T270:
        {
            return
            {
                { origin, Direction::DOWN, false },
                { { origin.x + 1, origin.y }, Direction::UP, true },
                { { origin.x, origin.y + 1 }, Direction::RIGHT, true },
                { { origin.x + 1, origin.y + 1}, Direction::RIGHT, true },
            };
        }

        case IntersectionType::CROSS:
        {
            return
            {
                { origin, Direction::DOWN | Direction::LEFT, true },
                { { origin.x + 1, origin.y }, Direction::UP | Direction::LEFT, true },
                { { origin.x, origin.y + 1}, Direction::DOWN | Direction::RIGHT, true },
                { { origin.x + 1, origin.y + 1}, Direction::UP | Direction::RIGHT, true },
            };
        }
    }

    return {};
}

} // namespace tt