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
    // else it should gt deleted
    new(userdata) ItemPtr{fact->createItem(itemname)};

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

}

/**
 *
 * Create an Item from the specified name.
 *
 */
ItemPtr ItemFactory::createItem(const std::string&  objid,
                                const GameObjectCallbacks& callbacks)
{
    const std::string jsonFile =
        _resources.getFilename(fmt::format("items/{}.json", objid));

//    if( !boost::filesystem::exists(jsonFile) )
//    {
//        auto error = fmt::format("json file '{}' not found", jsonFile);
//        throw std::runtime_error(error);
//    }

    std::ifstream   file(jsonFile.c_str());
    nl::json        json = nl::json::parse(file, nullptr, false);

//    if (json.is_discarded())
//    {
//        auto error = fmt::format("json file '{}' could not be loaded", jsonFile);
//        throw std::runtime_error(error);
//    }

//    // TODO: function that validates the JSON has all the required fields
//    if (!json.contains("texture"))
//    {
//        auto error = fmt::format("json file '{}' does not have a 'texture' field", jsonFile);
//        throw std::runtime_error(error);
//    }

//    GameObjectInfo objinfo = json.get<tt::GameObjectInfo>();

    auto objinfo = getObjectInfo(objid);
    auto texture = _resources.cacheTexture(objinfo.texturefile);
    if (texture == nullptr)
    {
            auto error = fmt::format("texture file '{}' not found", objinfo.texturefile);
            throw std::runtime_error(error);
    }

//    std::string textureFile = fmt::format("items/{}.png", objid);
//    sf::Texture* texture = _resources.cacheTexture(textureFile);
//    if (texture == nullptr)
//    {
//        auto error = fmt::format("texture file '{}' not found", textureFile);
//        throw std::runtime_error(error);
//    }

    //
    // By default, scale item image to tile size.
    //
    int     width   = texture->getSize().x;
    int     height  = texture->getSize().y;
    float   scaleX  = DEFAULT_ITEM_WIDTH    / width;
    float   scaleY  = DEFAULT_ITEM_HEIGHT   / height;
    
    //
    // Optionally allow for item author to specify
    // size and scale.
    //
    if( json.find("image-attr") != json.end())
    {
        nl::json children = json["image-attr"];

        if (children.find("width") != children.end() &&
            children.find("height") != children.end() &&
            children.find("scale-x") != children.end() &&
            children.find("scale-y") != children.end())
        {

            width = json["image-attr"]["width"];
            height = json["image-attr"]["height"];
            scaleX = json["image-attr"]["scale-x"];
            scaleY = json["image-attr"]["scale-y"];
        }
    }
    
    auto item   = std::make_shared<Item>(
                                    objid,
                                    *texture,
                                    sf::Vector2i{ width, height },
                                    objinfo);

    item->setScale(scaleX, scaleY);

    //if(json.find("name") != json.end())
    //{
    //    item->setName(json["name"]);
    //}

    //if(json.find("description") != json.end())
    //{
    //    item->setDescription(json["description"]);
    //}

    if(json.find("obtainable") != json.end())
    {
        item->setObtainable(json["obtainable"]);
    }

    //item->callbacks = callbacks;

    return item;
}

ItemPtr ItemFactory::createItem2(const std::string& objid, const GameObjectInstanceInfo& instinfo)
{
    auto objinfo = getObjectInfo(objid);
    auto texture = _resources.cacheTexture(objinfo.texturefile);
    if (texture == nullptr)
    {
        auto error = fmt::format("texture file '{}' not found", objinfo.texturefile);
        throw std::runtime_error(error);
    }

    const auto& objinfo = _objectMap.at(objid);
    auto item = std::make_shared<Item>(objinfo, instinfo);
    return item;
}

GameObjectInfo ItemFactory::getObjectInfo(const std::string& objid)
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

    // 2022-02-21: This is ugly. I wanted to create a default contructor and a constructor
    // that accepted just a std::string, but nlohmman wouldn't let me!
    retval.id = objid;

    _objectMap.insert({objid, retval});
    return _objectMap.at(objid);
}

} // namespace tt
