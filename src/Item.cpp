#include <iostream>

#include <lua/lua.hpp>

#include "Item.h"
#include "TTLua.h"

namespace tt
{

void from_json(const nl::json& j, Item::Callbacks& i)
{
    if (j.contains("onPickup")) 
    {
        j.at("onPickup").get_to(i.onPickup);
    }

    if (j.contains("onDrop")) 
    {
        j.at("onDrop").get_to(i.onDrop);
    }

    if (j.contains("onUse")) 
    {
        j.at("onUse").get_to(i.onUse);
    }

    if (j.contains("onConsume")) 
    {
        j.at("onConsume").get_to(i.onConsume);
    }
}

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

int Item_setName(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    const auto name = lua_tostring(L, 2);
    item->setName(name);
    return 0;
}

int Item_getDescription(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushstring(L, item->getDescription().c_str());
    return 1;
}

int Item_setDescription(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    const auto name = lua_tostring(L, 2);
    item->setDescription(name);
    return 0;
}

int Item_isObtainable(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushboolean(L, item->isObtainable() ? 1 : 0);
    return 1;
}

int Item_setObtainable(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    const auto val = lua_toboolean(L, 2);
    item->setObtainable(val);
    return 0;
}

int Item_isActionable(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    lua_pushboolean(L, item->isActionable() ? 1 : 0);
    return 1;
}

int Item_setActionable(lua_State* L)
{
    auto item = tt::checkObject<Item>(L);
    const auto val = lua_toboolean(L, 2);
    item->setActionable(val);
    return 0;
}

}

const struct luaL_Reg Item::LuaMethods[] =
{
    {"id", Item_getId},
    {"name", Item_getName},
    {"setName", Item_setName},
    {"description", Item_getDescription},
    {"setDescription", Item_setDescription},
    {"obtainable", Item_isObtainable},
    {"setObtainable", Item_setObtainable},
    {"actionable", Item_isActionable},
    {"setActionable", Item_setActionable},
    {nullptr, nullptr}
};

Item::Item( const std::string&  id,
            const sf::Texture& texture,
            const sf::Vector2i& size )
    : AnimatedSprite(texture, size),
        _id { id }
{
}

std::string Item::getID() const
{
    return _id;
}

void Item::setName(const std::string& s)
{
    _name = s;
}

std::string Item::getName() const
{
    return _name;
}

void Item::setDescription(const std::string& s)
{
    _description = s;
}

std::string Item::getDescription() const
{
    return _description;
}

void Item::setObtainable(bool b)
{
    _isObtainable = b;
}

bool Item::isObtainable() const
{
    return _isObtainable;
}


bool Item::isActionable() const
{
    return _isActionable;
}

void Item::setActionable(bool b)
{
    _isActionable = b;
}

std::string Item::getActionRequiresItem()
{
    return _actionRequiresItem;
}

void Item::setActionRequiresItem(const std::string& s)
{
    _actionRequiresItem = s;
}

std::string Item::getActionProvidesItem()
{
    return _actionProvidesItem;
}

void Item::setActionProvidesItem(const std::string& s)
{
    _actionProvidesItem = s;
}

std::string Item::getActionFailureMsg()
{
    return _actionFailureMsg;
}

void Item::setActionFailureMsg(const std::string& s)
{
    _actionFailureMsg = s;
}

std::string Item::getActionSuccessMsg()
{
    return _actionSuccessMsg;
}

void Item::setActionSuccessMsg(const std::string& s)
{
    _actionSuccessMsg = s;
}
 
}
