#include <iostream>

#include <lua.hpp>

#include "Item.h"
#include "TTLua.h"

namespace tt
{

namespace
{

int Item_getId(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushstring(L, item->getID().c_str());
    return 1;
}

int Item_getName(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushstring(L, item->getName().c_str());
    return 1;
}

int Item_getDescription(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushstring(L, item->getDescription().c_str());
    return 1;
}

int Item_isObtainable(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushboolean(L, item->obtainable() ? 1 : 0);
    return 1;
}

int Item_setObtainable(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    const auto val = lua_toboolean(L, 2);
    item->setObtainable(val);
    return 0;
}

}

const struct luaL_Reg Item::LuaMethods[] =
{
    {"id", Item_getId},
    {"name", Item_getName},
    {"description", Item_getDescription},
    {"obtainable", Item_isObtainable},
    {"setObtainable", Item_setObtainable},
    {nullptr, nullptr}
};

std::string Item::getID() const
{
    return this->_objectInfo.id;
}

std::string Item::getName() const
{
    return _objectInfo.name;
}

std::string Item::getDescription() const
{
    return _objectInfo.description;
}

}

