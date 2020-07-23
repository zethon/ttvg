#include <iostream>
#include "Item.h"

namespace tt
{

    Item::Item( std::string id,
                sf::Texture texture,
                const sf::Vector2i& size,
                nl::json json)
        : AnimatedSprite(texture, size),
            _json   { json },
            _id     { id }
    {

    }

    std::string Item::getName() { 

        //
        // Why is it that the json object can be treated as a top level
        // associative array when this is created in ItemFactory but 
        // after it is assigned to the member variable in this object
        // instance, it becomes a json array at the top level, 
        // and we must use _json[0] to get the first item?
        //
        return (std::string)(_json[0].at("name"));
    }


    std::string Item::getDescription() { 
        return (std::string)(_json[0].at("description"));
    }

    bool Item::isObtainable() { 
        if(_json[0]["obtainable"].is_null()) {
            return false;
        }
        return (bool)(_json[0].at("obtainable"));
    }

}
