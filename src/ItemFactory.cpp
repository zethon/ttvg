#include <boost/filesystem.hpp>

#include <lua/lua.hpp>

#include <fmt/core.h>

#include "Item.h"
#include "ItemFactory.h"

namespace tt
{

int ItemFactory_createItem(lua_State* L)
{
    //auto gamescreen = GameScreen::l_get(L);
    //
    //auto temp = static_cast<Scene**>(luaL_checkudata(L, 1, Scene::CLASS_NAME));
    //auto scene = *temp;
    //lua_pushstring(L, scene->name().c_str());
    return 1;
}

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
ItemPtr ItemFactory::createItem(const std::string&  name,
                                const sf::Vector2f& position)
{

    std::string jsonFile =
        _resources.getFilename(fmt::format("items/{}.json", name));

    if( !boost::filesystem::exists(jsonFile) )
    {
        auto error = fmt::format("file '{}' not found", jsonFile);
        throw std::runtime_error(error);
    }

    std::ifstream   file(jsonFile.c_str());
    nl::json        json;

    if(file.is_open())
    {
        file >> json;
    }

    std::string textureFile = fmt::format("items/{}.png", name);
    sf::Texture* texture = _resources.cacheTexture(textureFile);

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
                                    name, 
                                    *texture, 
                                    sf::Vector2i{ width, height } );

    item->setScale(scaleX, scaleY);
    item->setPosition(position);

    if(json.find("name") != json.end())
    {
        item->setName(json["name"]);
    }

    if(json.find("description") != json.end())
    {
        item->setDescription(json["description"]);
    }

    if(json.find("obtainable") != json.end())
    {
        item->setObtainable(json["obtainable"]);
    }

    //
    // Set actionable attributes
    //
    if(json.find("actionable") != json.end())
    {
        item->setActionable(json["actionable"]);
    }

    if(json.find("action-requires-item") != json.end())
    {
        item->setActionRequiresItem(json["action-requires-item"]);
    }

    if(json.find("action-provides-item") != json.end())
    {
        item->setActionProvidesItem(json["action-provides-item"]);
    }

    if(json.find("action-failure-msg") != json.end())
    {
        item->setActionFailureMsg(json["action-failure-msg"]);
    }

    if(json.find("action-success-msg") != json.end())
    {
        item->setActionSuccessMsg(json["action-success-msg"]);
    }

    return item;
}

} // namespace tt
