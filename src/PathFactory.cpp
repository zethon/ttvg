#include "PathFactory.h"

namespace tt
{

namespace
{

///
/// \brief  Free function that advances the passed in vector
///         by a single unit.
///
sf::Vector2i pathStep(const sf::Vector2i& point, Direction direction)
{
    switch (direction)
    {
        default: 
        break;

        case Direction::UP:
            return { point.x, point.y - 1 };

        case Direction::DOWN:
            return { point.x, point.y + 1 };

        case Direction::LEFT:
            return { point.x - 1, point.y };

        case Direction::RIGHT:
            return { point.x + 1, point.y };
    }

    return point;
}

} // anonymous namespace

////////////////////////////////////////////////////////////
PathFactory::PathFactory(const sf::Vector2i& size)
    : _size{ size }
{
    assert(_size.x > 0);
    assert(_size.y > 0);
}

////////////////////////////////////////////////////////////
void PathFactory::makeRiboPath(Path& path) const
{
    assert(_edges.size() > 0);
    auto start = tt::select_randomly(_edges);
    path.points().emplace_back(start->point);

    bool canDecide = true;
    auto currentDirection = static_cast<Direction>(start->turn);
    auto currentPoint = pathStep(start->point, currentDirection);

    while (currentPoint.x >= 0 && currentPoint.x < _size.x
        && currentPoint.y >= 0 && currentPoint.y < _size.y)
    {
        // TODO: should this search be done if we cannot make a decision?
        auto temp = std::find_if(_turns.begin(), _turns.end(), 
            [&currentPoint](const TurningPoint& inter)
            {
                return currentPoint == inter.point;
            });
        
        if (canDecide && (temp != _turns.end()))
        {
            if (temp->decisionPoint)
            {
                static std::random_device rd;
                static std::mt19937 gen(rd());

                std::uniform_int_distribution<> dis(0, 1);
                if (dis(gen) > 0)
                {
                    if (temp->turn != Direction::UP && temp->turn != Direction::DOWN
                        && temp->turn != Direction::LEFT && temp->turn != Direction::RIGHT)
                    {
                        currentDirection = static_cast<Direction>(temp->turn ^ currentDirection);
                    }
                    else
                    {
                        currentDirection = static_cast<Direction>(temp->turn);
                    }

                    path.points().emplace_back(currentPoint);
                    canDecide = false;
                }
            }
            else
            {
                currentDirection = static_cast<Direction>(temp->turn);
                path.points().emplace_back(currentPoint);
                canDecide = false;
            }
        }
        else
        {
            canDecide = true;
        }

        currentPoint = pathStep(currentPoint, currentDirection);
    }

    path.points().emplace_back(currentPoint);
}

////////////////////////////////////////////////////////////
Path PathFactory::makeRiboPath() const
{
    Path retval;
    makeRiboPath(retval);
    return retval;
}

} // namespace tt