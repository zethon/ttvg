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

        //
        // Initialize out highlight box. 
        // Note move this up into AnimatedSprite once it works.
        //
        auto left   = getGlobalBounds().left;
        auto top    = getGlobalBounds().top;

        _highlight.setPosition( left, top );
        _highlight.setFillColor(sf::Color::Transparent);
        _highlight.setOutlineThickness(2);
        _highlight.setOutlineColor(sf::Color(255, 255, 255));
        _highlight.setSize(sf::Vector2f(32, 32));

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

    void Item::setPosition(float x, float y)
    {
        AnimatedSprite::setPosition(x, y);

        _highlight.setPosition(x, y);
    }

    void Item::setPosition(const sf::Vector2f& position)
    {
        this->setPosition(position.x, position.y);
    }

    void Item::draw(sf::RenderTarget&   target,
                    sf::RenderStates    states) const
    {
        target.draw(static_cast<AnimatedSprite>(*this));
        target.draw(_highlight, states);
    }

}
