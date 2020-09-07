#pragma once
#include <memory>

#include <nlohmann/json.hpp>
#include "ResourceManager.h"

namespace nl = nlohmann;

namespace tt
{

class ItemFactory
{

ResourceManager&    _resources;

public:
    static constexpr auto CLASS_NAME = "ItemFactory";
    static const struct luaL_Reg LuaMethods[];
    
    ItemFactory(ResourceManager& resMgr);

    ItemPtr createItem(const std::string&  name,
                const sf::Vector2f& position,
                const Item::Callbacks& callbacks);

    ItemPtr createItem(const std::string& name, 
                sf::Vector2f& position)
    {
        return createItem(name, position, Item::Callbacks{});
    }

    ItemPtr createItem(const std::string& name)
    {
        return createItem(name, sf::Vector2f{}, Item::Callbacks{});
    }
};


} // namespace tt

