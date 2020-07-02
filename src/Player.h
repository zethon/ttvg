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
};

} // namespace tt