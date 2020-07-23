#pragma once

#include "AnimatedSprite.h"

namespace tt
{

class Player;
using PlayerPtr = std::shared_ptr<Player>;

class Player : public AnimatedSprite
{
public:
    using AnimatedSprite::AnimatedSprite;

    sf::Vector2f getGlobalCenter() const;

    float getGlobalLeft() const;
    float getGlobalRight() const;
    float getGlobalTop() const;
    float getGlobalBottom() const;

    void setGlobalLeft(float left);
    void setGlobalRight(float right);
    void setGlobalTop(float top);
    void setGlobalBottom(float bottom);

    //
    // Add an item to the player's inventory.
    //
    void addItem(const std::string& itemID);

    //
    // Get the player's inventory.
    //
    std::map<std::string, std::int32_t> getInventory();

private:

    //
    // Inventory. Map an item ID to a quantity.
    //
    std::map<std::string, std::int32_t> _inventory;

};

} // namespace tt
