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

            Item(       const std::string&  id,
            			const sf::Texture& texture, 
                        const sf::Vector2i& size,
                        const nl::json&     json );

            std::string getID() const;
            std::string getName() const;
            std::string getDescription() const;

            //
            // Check if an item can be picked up.
            //
            bool    isObtainable() const;

        private:

            nl::json    _json;
            std::string _id;

    };


} // namespace tt
