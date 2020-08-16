#include "Player.h"

namespace tt
{

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
