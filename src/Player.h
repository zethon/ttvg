#pragma once

#include "AnimatedSprite.h"
#include "Item.h"

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


    void addItem(const ItemPtr item);
    const std::vector<ItemPtr>& getInventory() const;

private:
    std::vector<ItemPtr> _inventory;

};

} // namespace tt
