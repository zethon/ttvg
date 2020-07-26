#include <boost/filesystem.hpp>

#include <fmt/core.h>

#include "Item.h"
#include "ItemFactory.h"

namespace tt
{

    //
    // Default item size.
    // Might want this to be the same as a "tile size" on the map.
    //
    constexpr auto DEFAULT_ITEM_WIDTH   = 32.0f;
    constexpr auto DEFAULT_ITEM_HEIGHT  = 32.0f;


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
        
        auto item   = std::make_shared<Item>(   name, 
                                            *texture, 
                                            sf::Vector2i{ width, height },
                                            json );



        item->setScale(scaleX, scaleY);
        item->setPosition(position);

        return item;
    }
} // namespace tt
