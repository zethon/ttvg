#include <boost/filesystem.hpp>

#include <fmt/core.h>

#include "VehicleFactory.h"

namespace tt
{

VehicleFactory::VehicleFactory(ResourceManager& resmgr, const std::string& mapname)
    : _resources { resmgr }
{
    std::string jsonfile =
        _resources.getFilename(fmt::format("maps/{}.json", mapname));

    if (!boost::filesystem::exists(jsonfile))
    {
        auto error = fmt::format("file '{}' not found", jsonfile);
        throw std::runtime_error(error);
    }

    std::ifstream file(jsonfile.c_str());
    file >> _json;
}

VehiclePtr VehicleFactory::createVehicle()
{
    return VehiclePtr();
}

sf::Vector2i stepDirection(const sf::Vector2i& point, Direction direction)
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

Path PathFactory::makeRandomPath() const
{
    Path retval;

    auto start = tt::select_randomly(_edges);
    retval.points().push_back(start->point);

    bool canDecide = true;
    auto currentDirection = static_cast<Direction>(start->turn);
    auto currentPoint = stepDirection(start->point, currentDirection);

    while (currentPoint.x >= 0 && currentPoint.x < _size.x
        && currentPoint.y >= 0 && currentPoint.y < _size.y)
    {
        auto temp = std::find_if(_turns.begin(), _turns.end(), 
            [&currentPoint](const Intersection& inter)
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
                    if (temp->turn != Direction::UP || temp->turn != Direction::DOWN
                        || temp->turn != Direction::LEFT || temp->turn != Direction::RIGHT)
                    {
                        currentDirection = static_cast<Direction>(temp->turn ^ currentDirection);
                    }
                    else
                    {
                        currentDirection = static_cast<Direction>(temp->turn);
                    }

                    retval.points().push_back(currentPoint);
                    canDecide = false;
                }
            }
            else
            {
                currentDirection = static_cast<Direction>(temp->turn);
                retval.points().push_back(currentPoint);
                canDecide = false;
            }
        }
        else
        {
            canDecide = true;
        }

        currentPoint = stepDirection(currentPoint, currentDirection);
    }

    retval.points().push_back(currentPoint);

    return retval;
}

} // namespace tt