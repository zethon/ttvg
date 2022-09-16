#include <lua.hpp>

#include "Player.h"
#include "TTLua.h"

namespace tt
{

void from_json(const nl::json & j, Player & i)
{
}

namespace
{

int Player_health(lua_State* L)
{
    auto player = checkObject<Player>(L);
    lua_pushnumber(L, player->health());
    return 1;
}

int Player_setHealth(lua_State* L)
{
    auto player = checkObject<Player>(L);
    auto health = lua_tointeger(L, 2);
    player->setHealth(static_cast<std::uint32_t>(health));
    return 0;
}

int Player_balance(lua_State* L)
{
    auto player = checkObject<Player>(L);
    lua_pushnumber(L, player->balance());
    return 1;
}

int Player_setBalance(lua_State* L)
{
    auto player = checkObject<Player>(L);
    float balance = static_cast<float>(lua_tonumber(L, 2));
    player->setBalance(balance);    
    return 0;
}

int Player_addItem(lua_State* L)
{
    auto player = checkObject<Player>(L);
    auto itemp = static_cast<ItemPtr*>(lua_touserdata(L, 2));
    player->addItem(*itemp);
    return 0;
}

int Player_hasItem(lua_State* L)
{
    auto player = checkObject<Player>(L);
    auto itemv = static_cast<ItemPtr*>(lua_touserdata(L, -2));
    lua_pushboolean(L, player->hasItem(*itemv) ? 1 : 0);
    return 1;
}

int Player_hasItemByName(lua_State* L)
{
    auto player = checkObject<Player>(L);
    const auto itemname = lua_tostring(L, 2);
    lua_pushboolean(L, player->hasItem(itemname) ? 1 : 0);
    return 1;
}

int Player_removeItem(lua_State* L)
{
    auto player = checkObject<Player>(L);
    auto itemp = static_cast<ItemPtr*>(lua_touserdata(L, 2));
    player->removeItem(*itemp);
    return 0;
}

int Player_removeItemByName(lua_State* L)
{
    auto player = checkObject<Player>(L);
    const auto itemname = lua_tostring(L, 2);
    player->removeItem(itemname);
    return 0;
}

int Player_setPosition(lua_State* L)
{
    auto player = checkObject<Player>(L);
    auto x = static_cast<float>(lua_tonumber(L, 2));
    auto y = static_cast<float>(lua_tonumber(L, 3));
    player->setPosition(sf::Vector2f{ x,y });
    return 0;
}

int Player_getPosition(lua_State* L)
{
    auto player = checkObject<Player>(L);
    const auto position = player->getPosition();
    lua_pushnumber(L, position.x);
    lua_pushnumber(L, position.y);
    return 2;
}

int Player_setState(lua_State* L)
{
    auto item = tt::checkObject<Player>(L);
    const auto state = lua_tostring(L, 2);
    item->setBaseState(state);
    return 0;
}

}

const struct luaL_Reg Player::LuaMethods[] =
{
    {"getBalance", Player_balance},
    {"setBalance", Player_setBalance},
    {"getHealth", Player_health},
    {"setHealth", Player_setHealth},

    {"addItem", Player_addItem},
    {"hasItem", Player_hasItem},
    {"hasItemByName", Player_hasItemByName},
    {"removeItem", Player_removeItem},
    {"removeItemByName", Player_removeItemByName},

    {"setPosition", Player_setPosition},
    {"getPosition", Player_getPosition},

    {"setBaseState", Player_setState},

    {nullptr, nullptr}
};

sf::Vector2f Player::getGlobalCenter() const
{
    const auto rect = getGlobalBounds();
    auto x = rect.left + (rect.width / 2);
    auto y = rect.top + (rect.height / 2);

    return { x, y };
}

sf::Vector2f Player::getGlobalCenter(   float dx,
                                        float dy ) const
{
    const auto rect = getGlobalBounds();
    auto x = rect.left + (rect.width / 2) + dx;
    auto y = rect.top + (rect.height / 2) + dy;

    return { x, y };
}


void Player::addItem(ItemPtr item)
{
    _inventory.push_back(item);
}

bool Player::hasItem(const std::string& s)
{
	for(const auto& i: _inventory)
    {
    	if(i->getID() == s)
        {
            return true;
        }
    }
	return false;
}

bool Player::hasItem(ItemPtr item)
{
    return std::find(
        _inventory.begin(), _inventory.end(), item) != _inventory.end();
}

void Player::removeItem(const std::string& s)
{
    for(auto it = _inventory.begin(); it != _inventory.end(); it++)
    {
        ItemPtr item = *it;
    	if(item->getID() == s)
        {
            _inventory.erase(it);
            return;
        }
    }
}

void Player::removeItem(ItemPtr item)
{
    auto it = std::find(
        _inventory.begin(), _inventory.end(), item);

    if (it != _inventory.end())
    {
        _inventory.erase(it);
    }
}

ItemPtr Player::getItemByName(const std::string& name)
{
    auto it = std::find_if(_inventory.begin(), _inventory.end(),
        [&name](ItemPtr i) { return i->getID() == name; });

    if (it != _inventory.end()) return *it;
    return {};
}

const std::vector<ItemPtr>& Player::getInventory() const
{
    return _inventory;
}

void Player::setHealth(std::int32_t h) 
{ 
    if (0 <= h && h <= 100)
    {
        _health = h;
    }
    else if (h > 100)
    {
        _health = 100;
    }
    else
    {
        assert(h < 0);
        _health = 0;
    }

    onSetHealth(_health);
}

void Player::reduceHealth(std::uint32_t amount)
{
    setHealth(_health - amount);
}

void Player::increaseHealth(std::uint32_t amount)
{
    setHealth(_health + amount);
}

void Player::setBalance(float c) 
{ 
    _cash = c;
    onSetCash(_cash);
}

bool Player::walking() const 
{ 
    return _moving; 
}

void Player::setWalking(bool walking) 
{ 
    _moving = walking;
    _movingTimer.restart();
}

void Player::punch()
{
    const auto state = fmt::format("{}_punch", tt::DirectionToString(this->direction()));
    this->interruptState(state);
}

void Player::spellcast()
{
    const auto state = fmt::format("{}_spellcast", tt::DirectionToString(this->direction()));
    this->queueState(state);
}

void Player::arrow()
{
    const auto state = fmt::format("{}_arrow", tt::DirectionToString(this->direction()));
    this->interruptState(state);
}

void Player::dance()
{
    const auto state = fmt::format("{}_dance", tt::DirectionToString(this->direction()));
    this->queueState(state);
}

std::uint16_t Player::update()
{
    // handle the base class animation
    Item::update();

    // check to see if we're walking and if it's time
    // to signal
    if (walking()
        && _movingTimer.getElapsedTime().asMilliseconds() > 75)
    {
        onMoveTimer();
        _movingTimer.restart();
    }

    return std::uint16_t();
}

} // namespace tt
