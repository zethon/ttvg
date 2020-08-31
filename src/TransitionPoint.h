#pragma once

#include <set>

#include <SFML/Graphics.hpp>

#include <nlohmann/json.hpp>

namespace nl = nlohmann;

namespace tt
{

struct Transition
{
    sf::Vector2f    position;
    bool            enabled;
    std::string     newscene;

    bool operator==(const Transition& other)
    {
        return position == other.position;
    }
};

inline bool operator<(const Transition& lhs, const Transition& rhs)
{
    if (lhs.position.x == rhs.position.x)
    {
        return lhs.position.y < rhs.position.y;
    }

    return lhs.position.x < rhs.position.x;
}

void from_json(const nl::json& j, Transition& t);

} // namespace tt