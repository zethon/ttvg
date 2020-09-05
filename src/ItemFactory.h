#pragma once
#include <memory>

#include <nlohmann/json.hpp>
#include "ResourceManager.h"

namespace nl = nlohmann;

namespace tt
{

class ItemFactory
{

ResourceManager&    _resources;

public:
    ItemFactory(ResourceManager& resMgr);

    ItemPtr createItem( const std::string&  name,
                        const sf::Vector2f& position );

    ItemPtr createItem(const std::string& name)
    {
        return createItem(name, {});
    }
};


} // namespace tt

