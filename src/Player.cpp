#include <lua/lua.hpp>

#include "Player.h"

namespace tt
{

[[maybe_unused]] Player* checkPlayerObj(lua_State* L, int index = 1)
{
    auto temp = static_cast<Player**>(luaL_checkudata(L, 1, Player::CLASS_NAME));
    return *temp;
}

int Player_health(lua_State* L)
{
    auto player = checkPlayerObj(L);
    lua_pushnumber(L, player->health());
    return 1;
}

int Player_setHealth(lua_State* L)
{
    auto player = checkPlayerObj(L);
    auto health = lua_tointeger(L, 2);
    player->setHealth(static_cast<std::uint32_t>(health));
    return 0;
}

int Player_balance(lua_State* L)
{
    auto player = checkPlayerObj(L);
    lua_pushnumber(L, player->balance());
    return 1;
}

int Player_setBalance(lua_State* L)
{
    auto player = checkPlayerObj(L);
    float balance = static_cast<float>(lua_tonumber(L, 2));
    player->setBalance(balance);    
    return 0;
}

int Player_hasItem(lua_State* L)
{
    auto player = checkPlayerObj(L);
    const auto itemname = lua_tostring(L, 2);
    lua_pushboolean(L, player->hasItem(itemname) ? 1 : 0);
    return 1;
}

int Player_removeItem(lua_State* L)
{
    auto player = checkPlayerObj(L);
    const auto itemname = lua_tostring(L, 2);
    player->removeItem(itemname);
    return 0;
}

const struct luaL_Reg Player::LuaMethods[] =
{
    {"getBalance", Player_balance},
    {"setBalance", Player_setBalance},
    {"getHealth", Player_health},
    {"setHealth", Player_setHealth},
    {"hasItem", Player_hasItem},
    {"removeItem", Player_removeItem},
    {nullptr, nullptr}
};

sf::Vector2f Player::getGlobalCenter() const
{
    const auto rect = getGlobalBounds();
    auto x = rect.left + (rect.width / 2);
    auto y = rect.top + (rect.height / 2);

    return { x, y };
}

float Player::getGlobalLeft() const
{
    return getGlobalBounds().left;
}

float Player::getGlobalRight() const
{
    return getGlobalBounds().left + getGlobalBounds().width;
}

float Player::getGlobalTop() const
{
    return getGlobalBounds().top;
}

float Player::getGlobalBottom() const
{
    return getGlobalBounds().top + getGlobalBounds().height;
}

void Player::setGlobalLeft(float left)
{
    auto bounds = getGlobalBounds();
    setPosition(left, bounds.top);
}

void Player::setGlobalRight(float right)
{
    auto bounds = getGlobalBounds();
    auto x = right - bounds.width;
    setPosition(x, bounds.top);
}

void Player::setGlobalTop(float top)
{
    auto bounds = getGlobalBounds();
    setPosition(bounds.left, top);
}

void Player::setGlobalBottom(float bottom)
{
    auto bounds = getGlobalBounds();
    auto y = bottom - bounds.height;
    setPosition(bounds.left, y);
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

const std::vector<ItemPtr>& Player::getInventory() const
{
    return _inventory;
}

void Player::setHealth(std::uint32_t h) 
{ 
    if (0 <= h && h <= 100)
    {
        _health = h;
        onSetHealth(_health);
    }
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


} // namespace tt
