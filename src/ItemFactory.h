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
                const ItemCallbacks& callbacks);

    ItemPtr createItem(const std::string& name)
    {
        return createItem(name, ItemCallbacks{});
    }

    ItemPtr createSceneItems(const std::string& id, const nl::json& sceneEl);
};


} // namespace tt

