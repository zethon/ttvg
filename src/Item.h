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
                        const sf::Texture&  texture, 
                        const sf::Vector2i& size );

            std::string getID() const;

            std::string getName() const;
            void        setName(const std::string& s);

            std::string getDescription() const;
            void        setDescription(const std::string& s);

            //
            // Check if an item can be picked up.
            //
            bool    isObtainable() const;
            void    setObtainable(bool b);

        private:

            std::string _id;
            std::string _name;
            std::string _description;
            bool        _isObtainable = false;

    };


} // namespace tt
