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
    ItemPtr ItemFactory::createItem(const std::string& name)
    {

        std::string jsonFile =
            _resources.getFilename(fmt::format("items/{}.json", name));

        std::string textureFile =
            _resources.getFilename(fmt::format("items/{}.png", name));

    	if( !boost::filesystem::exists(jsonFile) )
    	{
        	auto error = fmt::format("file '{}' not found", jsonFile);
        	throw std::runtime_error(error);
    	}

    	if( !boost::filesystem::exists(textureFile) )
        {
        	auto error = fmt::format("file '{}' not found", textureFile);
        	throw std::runtime_error(error);
        }

        auto texture    = *(_resources.load<sf::Texture>(textureFile));

    	std::ifstream   file(jsonFile.c_str());
        nl::json        json;

    	file >> json;

        auto item       = std::make_shared<Item>(
                                                    texture, 
                                                    sf::Vector2i{ 77, 41 },
                                                    json );

        return item;
    }

} // namespace tt
