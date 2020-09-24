#pragma once
#include <vector>
#include <variant>

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "AnimatedSprite.h"

namespace nl = nlohmann;

namespace tt
{

class Item;
using ItemPtr = std::shared_ptr<Item>;

struct ItemCallbacks
{
    std::string onPickup;
    std::string onDrop;
    std::string onUse;
    std::string onConsume;
};

enum class ItemFlags : std::uint16_t
{
    NONE = 0x0000,
    CONSUMABLE = 0x0001,
    WEAPON = 0x0002,
    INSTRUMENT = 0x0004,       // can be used for busking
};

enum Coordinates { X, Y };
using ItemCoordinate = std::variant<float, std::tuple<Coordinates, std::string>>;
struct ItemInfo
{
    ItemCoordinate  x;
    ItemCoordinate  y;
    ItemCallbacks   callbacks;
};

class Item : public AnimatedSprite
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

    //
    // Check if an item can be picked up.
    //
    bool    isObtainable() const;
    void    setObtainable(bool b);

    ItemCallbacks   callbacks;

private:

    std::string     _id;
    std::string     _name;
    std::string     _description;

    std::uint32_t   _flags = 0;

    bool        _isObtainable = false;
};

void from_json(const nl::json& j, ItemCallbacks& i);
void from_json(const nl::json& j, ItemInfo& i);

} // namespace tt
