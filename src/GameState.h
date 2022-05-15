#pragma

#include <string>

#include <SFML/System/Vector2.hpp>

#include <nlohmann/json.hpp>

#include "Player.h"
#include "TTUtils.h"

namespace tt
{

struct GameState;
using GameStatePtr = std::shared_ptr<GameState>;

void from_json(const nl::json& j, GameState& pl);
void to_json(nl::json& j, const GameState& b);

struct PlayerLocation;
void from_json(const nl::json& j, PlayerLocation& pl);
void to_json(nl::json& j, const PlayerLocation& b);

struct PlayerState;
void from_json(const nl::json& j, PlayerState& pl);
void to_json(nl::json& j, const PlayerState& b);

struct PlayerLocation
{
    std::string     map;
    sf::Vector2f    cords;
};

struct PlayerState
{
    std::uint32_t   health = 0;
    float           cash = 0.f;
};

struct GameState
{
    PlayerState     playerstate;
    PlayerLocation  location;
};

} // namespace tt
