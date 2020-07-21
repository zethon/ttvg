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

    std::string Item::getDescription() { return _json["description"]; }

}
