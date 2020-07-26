#pragma once
#include <memory>

#include <nlohmann/json.hpp>
#include "ResourceManager.h"

namespace nl = nlohmann;

namespace tt
{
    //
    // Default item size.
    // Might want this to be the same as a "tile size" on the map.
    //
    constexpr auto DEFAULT_ITEM_WIDTH   = 32.0f;
    constexpr auto DEFAULT_ITEM_HEIGHT  = 32.0f;

    using ItemPtr = std::shared_ptr<Item>;

    class ItemFactory
    {
        ResourceManager&    _resources;

        public:

            ItemFactory(ResourceManager& resMgr);
    
            ItemPtr createItem( const std::string&  name,
                                const sf::Vector2f& position );

    };


} // namespace tt

