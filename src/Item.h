#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "AnimatedSprite.h"

namespace nl = nlohmann;

namespace tt
{

class Item;
using ItemPtr = std::shared_ptr<Item>;

class Item : public AnimatedSprite
{

public:
    static constexpr auto CLASS_NAME = "Item";
    static const struct luaL_Reg LuaMethods[];

    struct Callbacks
    {
        std::string onPickup;
        std::string onDrop;
        std::string onUse;
        std::string onConsume;
    };

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

    //
    // Check if an action can be performed on this item.
    //
    bool        isActionable() const;
    void        setActionable(bool b);

    std::string getActionRequiresItem();
    void        setActionRequiresItem(const std::string& s);
    std::string getActionProvidesItem();
    void        setActionProvidesItem(const std::string& s);
    std::string getActionFailureMsg();
    void        setActionFailureMsg(const std::string& s);
    std::string getActionSuccessMsg();
    void        setActionSuccessMsg(const std::string& s);

    Callbacks   callbacks;

private:

    std::string _id;
    std::string _name;
    std::string _description;
    bool        _isObtainable = false;

    bool        _isActionable = false;
    std::string _actionRequiresItem;
    std::string _actionProvidesItem;
    std::string _actionFailureMsg;
    std::string _actionSuccessMsg;


};

void from_json(const nl::json& j, Item::Callbacks& i);

} // namespace tt
