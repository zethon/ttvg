#include <boost/filesystem.hpp>

#include <fmt/core.h>

#include "Item.h"
#include "ItemFactory.h"

namespace tt
{

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
        // auto texture = *(_resources.load<sf::Texture>(textureFile));
        // auto image = *(_resources.load<sf::Image>(textureFile));
        
        // sf::Texture texture;
        // texture.loadFromImage(image);

        auto texture = *(_resources.load<sf::Texture>(textureFile));

        if( !boost::filesystem::exists(jsonFile) )
    	{
        	auto error = fmt::format("file '{}' not found", jsonFile);
        	throw std::runtime_error(error);
    	}

    	std::ifstream   file(jsonFile.c_str());
        nl::json        json;

    	file >> json;

        //
        // Debug
        //
        std::cout << 
            "Item desc: " + (std::string)(json.at("description")) << std::endl;

        int width       = json["image-attr"]["width"];
        int height      = json["image-attr"]["height"];
        float scaleX    = json["image-attr"]["scale-x"];
        float scaleY    = json["image-attr"]["scale-y"];

        auto item   = std::make_shared<Item>(   texture, 
                                                sf::Vector2i{ width, height },
                                                json );
        
        item->setScale(scaleX, scaleY);
        item->setPosition(position);

        return item;
    }

} // namespace tt
