#pragma once

#include <boost/signals2.hpp>

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

    std::uint32_t health() const { return _health; }
    void setHealth(std::uint32_t h);
    void reduceHealth(std::uint32_t amount);
    void increaseHealth(std::uint32_t amount);
    boost::signals2::signal<void(std::uint32_t health)> onSetHealth;

    float balance() const { return _cash; }
    void setBalance(float c);
    boost::signals2::signal<void(float cash)> onSetCash;

private:
    std::vector<ItemPtr>    _inventory;

    std::uint32_t           _health = 100;
    float                   _cash = 40.0f;

};

} // namespace tt
