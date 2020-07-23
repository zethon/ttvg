#include <iostream>
#include "Item.h"

namespace tt
{

    Item::Item( sf::Texture texture,
                const sf::Vector2i& size,
                nl::json json)
        : AnimatedSprite(texture, size),
            _json { json }
    {

    }

    std::string Item::getDescription() { 

        //
        // Why is it that the json object can be treated as a top level
        // associative array when this is created in ItemFactory but 
        // after it is assigned to the member variable in this object
        // instance, it becomes a json array at the top level, 
        // and we must use _json[0] to get the
        // first item?
        //
        return (std::string)(_json[0].at("description"));
    }

}
