#pragma once
#include <string>
#include <optional>

#include <lua/lua.hpp>

#include <nlohmann/json.hpp>

#include <SFML/Graphics.hpp>

#include "Transition.h"

namespace tt
{

struct Zone
{
    static constexpr auto CLASS_NAME = "Zone";
    static const struct luaL_Reg LuaMethods[];

    struct Callbacks
    {
        std::string onSelect;
    };

    std::string                 name;
    std::string                 description;
    sf::FloatRect               rect;
    std::optional<Transition>   transition;
    Callbacks                   callbacks;
};

void from_json(const nl::json& j, Zone& z);

} // namespace