#include "TTUtils.h"
#include "GameState.h"

namespace tt
{

void from_json(const nl::json& j, GameState& gs)
{
    j["player"].get_to(gs.playerstate);
    j["location"].get_to(gs.location);
}

void to_json(nl::json& j, const GameState& gs)
{
    j["player"] = gs.playerstate;
    j["location"] = gs.location;
}

void from_json(const nl::json& j, PlayerLocation& pl)
{
    j["map"].get_to(pl.map);
    j["cords"].get_to(pl.cords);
}

void to_json(nl::json& j, const PlayerLocation& pl)
{
    j["map"] = pl.map;
    j["cords"] = pl.cords;
}

void from_json(const nl::json& j, PlayerState& ps)
{
    j["health"].get_to(ps.health);
    j["cash"].get_to(ps.cash);
}

void to_json(nl::json& j, const PlayerState& ps)
{
    j["health"] = ps.health;
    j["cash"] = ps.cash;
}

} // namespace tt
