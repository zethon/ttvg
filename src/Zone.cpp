#include <lua.hpp>

#include "Zone.h"
#include "TTLua.h"

namespace tt
{

void from_json(const nl::json& j, Zone& z)
{
    j.at("name").get_to(z.name);
    if (j.contains("description"))
    {
        j.at("description").get_to(z.description);
    }
    
    if (j.contains("transition"))
    {
        z.transition = j["transition"].get<Transition>();
    }

    if (j.contains("onSelect"))
    {
        j.at("onSelect").get_to(z.callbacks.onSelect);
    }

    if (j.contains("barrier"))
    {
        z.barrier = j.at("barrier").get<bool>();
    }
}

namespace
{

int Zone_name(lua_State* L)
{
    auto zone = checkObject<Zone>(L);
    lua_pushstring(L, zone->name.c_str());
    return 1;
}

} // anonymouse namespace

const struct luaL_Reg Zone::LuaMethods[] =
{
    {"getName", Zone_name},
    {nullptr, nullptr}
};

} // namespace