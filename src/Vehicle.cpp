#include "Vehicle.h"

namespace tt
{

Vehicle::Vehicle(sf::Texture texture, const sf::Vector2i& size)
    : AnimatedSprite(texture, size)
{
    _texture.setSmooth(true);
    setSource(0, 2);
    setScale(1.86f, 1.86f);
    setState(AnimatedSprite::State::ANIMATED);
}

} // namespace tt