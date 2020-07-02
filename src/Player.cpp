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

} // namespace tt