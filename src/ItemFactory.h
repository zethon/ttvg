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

    ItemPtr createItem(const GameObjectInstanceInfo& instinfo);
    ItemPtr createItem(const std::string& objid);

private:
    GameObjectInfo& getObjectInfoRef(const std::string& name);
};


} // namespace tt

