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
GameObjectInfoMap   _objectMap;

public:
    static constexpr auto CLASS_NAME = "ItemFactory";
    static const struct luaL_Reg LuaMethods[];
    
    ItemFactory(ResourceManager& resMgr);

    ItemPtr createItem(const std::string&  name,
                const GameObjectCallbacks& callbacks);

    ItemPtr createItem(const std::string& name)
    {
        return createItem(name, GameObjectCallbacks{});
    }

    ItemPtr createItem2(const std::string& name, const GameObjectInstanceInfo& instinfo);

    GameObjectInfo& getObjectInfoRef(const std::string& name);
};


} // namespace tt

