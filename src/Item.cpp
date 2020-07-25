#include <iostream>
#include "Item.h"

namespace tt
{
    Item::Item( const std::string&  id,
                sf::Texture         texture,
                const sf::Vector2i& size,
                const nl::json&     json )
        : AnimatedSprite(texture, size),
            _json   { json },
            _id     { id }
    {

    }

    std::string Item::getID() const
    {
        return _id;
    }

    std::string Item::getName() const
    {

        //
        // Why is it that the json object can be treated as a top level
        // associative array when this is created in ItemFactory but 
        // after it is assigned to the member variable in this object
        // instance, it becomes a json array at the top level, 
        // and we must use _json[0] to get the first item?
        //
        return _json[0].at("name");
    }

    std::string Item::getDescription() const
    {
        return _json[0].at("description");
    }

    bool Item::isObtainable() const
    {
        if(_json[0].find("obtainable") == _json[0].end())
        {
            return false;
        }
        return _json[0].at("obtainable");
    }

}
