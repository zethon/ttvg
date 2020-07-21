#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "AnimatedSprite.h"

namespace nl = nlohmann;

namespace tt
{

    class Item : public AnimatedSprite
    {

        public:

            Item(       sf::Texture texture, 
                        const sf::Vector2i& size,
                        nl::json json);

            std::string getDescription();

        private:
   
            nl::json    _json;

    };


} // namespace tt
