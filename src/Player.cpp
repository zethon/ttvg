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


void Player::addItem(const std::string& itemID)
{
    _inventory[itemID]++;
}


const std::map<std::string, std::int32_t>& Player::getInventory() const
{
    return _inventory;
}


} // namespace tt
