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

class Item : public GameObject
{

public:
    static constexpr auto CLASS_NAME = "Item";
    static const struct luaL_Reg LuaMethods[];

    Item(const GameObjectInfo& obj, const GameObjectInstanceInfo& inst)
        : GameObject(obj, inst)
    {
        // nothing to do
    }

    std::string getID() const;

    std::string getName() const;
    void        setName(const std::string& s);

    std::string getDescription() const;
    void        setDescription(const std::string& s);

private:

};

} // namespace tt
