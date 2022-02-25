#include <boost/filesystem.hpp>

#include <lua.hpp>

#include <fmt/core.h>

#include "Item.h"
#include "ItemFactory.h"
#include "TTLua.h"

namespace tt
{

namespace
{

ItemFactory* checkItemFactory(lua_State* L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, ITEMFACTORY_LUA_IDX);
    int type = lua_type(L, -1);
    if (type != LUA_TLIGHTUSERDATA)
    {
        return nullptr;
    }

    ItemFactory* state = static_cast<ItemFactory*>(lua_touserdata(L, -1));
    if (!state)
    {
        return nullptr;
    }

    lua_pop(L, 1);
    return state;
}

int ItemFactory_createItem(lua_State* L)
{
    auto fact = checkItemFactory(L);
    const auto itemname = lua_tostring(L, 1);

    std::size_t size = sizeof(ItemPtr);
    void* userdata = lua_newuserdata(L, size);

    // create a shared_ptr in the space Lua allocated
    // for us, so if we never assign this to anyone/thing
    // else it should get deleted
    //new(userdata) ItemPtr{fact->createItem(itemname)};
    throw std::runtime_error("implement LUA createitem");

	luaL_setmetatable(L, Item::CLASS_NAME);
    return 1;
}

} // anonymous namespace

const struct luaL_Reg ItemFactory::LuaMethods[] =
{
    {"createItem", ItemFactory_createItem},
    {nullptr, nullptr}
};

//
// Default item size.
// Might want this to be the same as a "tile size" on the map.
//
constexpr auto DEFAULT_ITEM_WIDTH   = 36.0f;
constexpr auto DEFAULT_ITEM_HEIGHT  = 36.0f;


ItemFactory::ItemFactory(ResourceManager& resMgr)
    : _resources { resMgr }
{
    // nothing to do
}

/**
 *
 * Create an Item from the specified name.
 *
 */
ItemPtr ItemFactory::createItem(const std::string& objid,
                                 const GameObjectInstanceInfo& instinfo,
                                 const GameObjectInstanceInfo& groupinfo)
{
    auto& objinfo = getObjectInfoRef(objid);
    auto texture = _resources.cacheTexture(objinfo.texturefile);
    if (texture == nullptr)
    {
        auto error = fmt::format("texture file '{}' not found", objinfo.texturefile);
        throw std::runtime_error(error);
    }

    auto item = std::make_shared<Item>(objinfo, instinfo);   
    return item;
}

GameObjectInfo& ItemFactory::getObjectInfoRef(const std::string& objid)
{
    // load or cache the object info
    if (_objectMap.find(objid) != _objectMap.end())
    {
        return _objectMap.at(objid);
    }

    const auto jsonFile = _resources.getFilename(fmt::format("items/{}.json", objid));

    if( !boost::filesystem::exists(jsonFile) )
    {
        auto error = fmt::format("json file '{}' not found", jsonFile);
        throw std::runtime_error(error);
    }

    std::ifstream   file(jsonFile.c_str());
    nl::json        j = nl::json::parse(file, nullptr, false);

    if (j.is_discarded() || !j.is_object())
    {
        auto error = fmt::format("json file '{}' could not be loaded", jsonFile);
        throw std::runtime_error(error);
    }

    if (j.find("name") == j.end())
    {
        auto error = fmt::format("json file '{}' does not have a 'name' field", jsonFile);
        throw std::runtime_error(error);
    }

    GameObjectInfo retval = j.get<tt::GameObjectInfo>();
    retval.id = objid;

    if (retval.texturefile.empty())
    {
        retval.texturefile = fmt::format("items/{}.png", objid);
    }
    
    if (auto texture = _resources.cacheTexture(retval.texturefile);
            texture == nullptr)
    {
            auto error = fmt::format("texture file '{}' not found", retval.texturefile);
            throw std::runtime_error(error);
    }
    else
    {
        retval.texture = texture;
    }

    if (!retval.size.has_value())
    {
        retval.size = retval.texture->getSize();
    }
    
    _objectMap.insert({objid, retval});
    return _objectMap.at(objid);
}

// this is when I wish C++ had reflection so I could iterate the members
// but instead we have to do this manually
GameObjectInstanceInfo resolveDefaults(const GameObjectInstanceInfo& instinfo,
                                       const GameObjectInstanceInfo& groupinfo)
{
    GameObjectInstanceInfo retval { instinfo }; // copy!

    if (!retval.x.has_value() && groupinfo.x.has_value())
    {
        retval.x = groupinfo.x;
    }

    if (!retval.y.has_value() && groupinfo.y.has_value())
    {
        retval.y = groupinfo.y;
    }

    if (!retval.respawn.has_value() && groupinfo.respawn.has_value())
    {
        retval.respawn = groupinfo.respawn;
    }

    if (!retval.scale.has_value() && groupinfo.scale.has_value())
    {
        retval.scale = groupinfo.scale;
    }

    if (!retval.callbacks.onSelect.has_value() && groupinfo.callbacks.onSelect.has_value())
    {
        retval.callbacks.onSelect = groupinfo.callbacks.onSelect;
    }

    return retval;
}


} // namespace tt
