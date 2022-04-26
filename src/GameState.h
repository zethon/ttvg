#pragma

#include <string>

#include <SFML/System/Vector2.hpp>

#include <nlohmann/json.hpp>

namespace tt
{

struct PlayerLocation
{
    std::string     map;
    sf::Vector2f    location;
};

struct GameState
{
    PlayerLocation  location;
};

} // namespace tt
