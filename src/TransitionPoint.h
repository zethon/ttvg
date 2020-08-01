#pragma once

#include <set>

#include <SFML/Graphics.hpp>

#include <nlohmann/json.hpp>

namespace nl = nlohmann;

namespace tt
{

struct Transition
{
    enum class Type
    {
        MANUAL,
        AUTO
    };

    std::string     id;
    std::string     description;
    sf::Vector2f    position;
    bool            enabled;
    Type            type;
};

void to_json(nl::json& j, const Transition& t);
void from_json(const nl::json& j, Transition& t);

struct transition_compare
{

bool operator()(const tt::Transition& lhs, const tt::Transition& rhs) const
{
    if (lhs.position.x == rhs.position.x)
    {
        return lhs.position.y < rhs.position.y;
    }

    return lhs.position.x < rhs.position.x;
}

};

using TransitionSet = std::set<Transition, transition_compare>;


} // namespace tt