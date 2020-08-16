#include <iostream>
#include "Item.h"

namespace tt
{
    Item::Item( const std::string&  id,
                const sf::Texture& texture,
                const sf::Vector2i& size )
        : AnimatedSprite(texture, size),
            _id { id }
    {
    }

    std::string Item::getID() const
    {
        return _id;
    }

    void Item::setName(const std::string& s)
    {
        _name = s;
    }

    std::string Item::getName() const
    {
        return _name;
    }

    void Item::setDescription(const std::string& s)
    {
        _description = s;
    }

    std::string Item::getDescription() const
    {
        return _description;
    }

    void Item::setObtainable(bool b)
    {
        _isObtainable = b;
    }

    bool Item::isObtainable() const
    {
        return _isObtainable;
    }


	bool Item::isActionable() const
    {
        return _isActionable;
    }

    void Item::setActionable(bool b)
    {
        _isActionable = b;
    }

    std::string Item::getActionRequiresItem()
    {
        return _actionRequiresItem;
    }

    void Item::setActionRequiresItem(const std::string& s)
    {
        _actionRequiresItem = s;
    }

    std::string Item::getActionProvidesItem()
    {
        return _actionProvidesItem;
    }

    void Item::setActionProvidesItem(const std::string& s)
    {
        _actionProvidesItem = s;
    }

    std::string Item::getActionFailureMsg()
    {
        return _actionFailureMsg;
    }

    void Item::setActionFailureMsg(const std::string& s)
    {
        _actionFailureMsg = s;
    }

    std::string Item::getActionSuccessMsg()
    {
        return _actionSuccessMsg;
    }

    void Item::setActionSuccessMsg(const std::string& s)
    {
        _actionSuccessMsg = s;
    }
 
}
