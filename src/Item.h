#pragma once
#include <vector>
#include <variant>
#include <optional>

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "GameObject.h"

namespace nl = nlohmann;

namespace tt
{

class Item;
using ItemPtr = std::shared_ptr<Item>;

struct ItemInstanceInfo
{
    std::string             id;

    // a null x,y means that the coordinate was not specified, 
    // and a value of -1 means it should be picked randomly
    std::optional<float>    x;
    std::optional<float>    y;
    std::optional<float>    respawn;

    GameObjectCallbacks     callbacks;
};

class Item : public GameObject
{

public:
    static constexpr auto CLASS_NAME = "Item";
    static const struct luaL_Reg LuaMethods[];

    Item(       const std::string&  id,
                const sf::Texture&  texture, 
                const sf::Vector2i& size );

    std::string getID() const;

    std::string getName() const;
    void        setName(const std::string& s);

    std::string getDescription() const;
    void        setDescription(const std::string& s);

    bool    isObtainable() const;
    void    setObtainable(bool b);

    GameObjectCallbacks   callbacks;

    void setInfo(const ItemInstanceInfo& info) { _itemInfo = info; }
    ItemInstanceInfo info() const { return _itemInfo; }

private:

    std::string     _id;
    std::string     _name;
    std::string     _description;

    std::uint32_t   _flags = 0;
    bool            _isObtainable = false;

    ItemInstanceInfo        _itemInfo;
};

void from_json(const nl::json& j, ItemInstanceInfo& i);

} // namespace tt
