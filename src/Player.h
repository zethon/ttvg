#pragma once

#include <boost/signals2.hpp>

#include <nlohmann/json.hpp>

#include "Item.h"

namespace tt
{

class Player;
using PlayerPtr = std::shared_ptr<Player>;

void from_json(const nl::json& j, Player& i);
void to_json(nl::json& j, const Player& p);

class Player : public Item
{
public:
    static constexpr auto CLASS_NAME = "Player";
    static const struct luaL_Reg LuaMethods[];

    friend void from_json(const nl::json& j, Player& i);

    using Item::Item;

    sf::Vector2f getGlobalCenter() const;

    void addItem(ItemPtr item);
    bool hasItem(const std::string& s);
    bool hasItem(ItemPtr item);
    void removeItem(const std::string& s);
    void removeItem(ItemPtr item);
    ItemPtr getItemByName(const std::string& name);

    const std::vector<ItemPtr>& getInventory() const;

    std::uint32_t health() const { return _health; }
    void setHealth(std::int32_t h);
    void reduceHealth(std::uint32_t amount);
    void increaseHealth(std::uint32_t amount);
    boost::signals2::signal<void(std::uint32_t health)> onSetHealth;

    float balance() const { return _cash; }
    void setBalance(float c);
    boost::signals2::signal<void(float cash)> onSetCash;

    bool walking() const;
    void setWalking(bool v);

    Direction direction() const { return _direction; }
    void setDirection(Direction val) { _direction = val; }

    void punch();
    void spellcast();
    void arrow();
    void dance();

    std::uint16_t update() override;

public: // signals

    boost::signals2::signal<void(void)> onMoveTimer;

private:
    std::vector<ItemPtr>    _inventory;
    Direction               _direction = Direction::DOWN;
    std::uint32_t           _health = 100;
    float                   _cash = 40.0f;

    sf::Clock               _movingTimer;
    bool                    _moving = false;

};

} // namespace tt
