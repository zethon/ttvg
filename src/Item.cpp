#include <iostream>
#include "Item.h"

namespace tt
{
    Item::Item(	const std::string&  id,
				const sf::Texture& texture,
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
        return _json["name"];
    }

    std::string Item::getDescription() const
    {
        return _json["description"];
    }

    bool Item::isObtainable() const
    {
        if(_json[0].find("obtainable") == _json[0].end())
        {
            return false;
        }
        return _json["obtainable"];
    }

}
