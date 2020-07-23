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

        std::string textureFile = fmt::format("items/{}.png", name);
        auto texture = *(_resources.load<sf::Texture>(textureFile));

        if( !boost::filesystem::exists(jsonFile) )
        {
            auto error = fmt::format("file '{}' not found", jsonFile);
            throw std::runtime_error(error);
        }

        std::ifstream   file(jsonFile.c_str());
        nl::json        json;

        if(file.is_open()) {
            file >> json;
        }

        //
        // Variables used for size and scale.
        //
        int     width;
        int     height;
        float   scaleX;
        float   scaleY;

        //
        // Optionally allow for item author to specify
        // size and scale.
        //
        if( !json["image-attr"].is_null()               &&
            !json["image-attr"]["width"].is_null()      &&
            !json["image-attr"]["height"].is_null()     &&
            !json["image-attr"]["scale-x"].is_null()    &&
            !json["image-attr"]["scale-y"].is_null()        ) {

            width   = json["image-attr"]["width"];
            height  = json["image-attr"]["height"];
            scaleX  = json["image-attr"]["scale-x"];
            scaleY  = json["image-attr"]["scale-y"];

        } else {
            
            //
            // By default, scale to tile size.
            //

            width   = texture.getSize().x;
            height  = texture.getSize().y;
            scaleX  = DEFAULT_ITEM_WIDTH    / width;
            scaleY  = DEFAULT_ITEM_HEIGHT   / height;
        }

        auto item   = std::make_shared<Item>(   name,
                                                texture, 
                                                sf::Vector2i{ width, height },
                                                json );
      

        item->setScale(scaleX, scaleY);
        item->setPosition(position);

        return item;
    }

} // namespace tt
