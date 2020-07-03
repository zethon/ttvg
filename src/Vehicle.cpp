#include "Background.h"
#include "Vehicle.h"

namespace tt
{

Vehicle::Vehicle(sf::Texture texture, const sf::Vector2i& size, BackgroundSharedPtr bg)
    : AnimatedSprite(texture, size),
      _background { bg }
{
    _path.push_back(51,263);
    _path.push_back(156,263);
    _path.push_back(156,335);
    _path.push_back(51,335);

    _texture.setSmooth(true);
    setSource(0, 2);
    setScale(1.86f, 1.86f);
    setState(AnimatedSprite::State::ANIMATED);

    auto globalPos = getGlobalXY(161.f, 287.f, _background->tilesize());
    setPosition(globalPos);
}

std::uint16_t Vehicle::timestep()
{
    AnimatedSprite::timestep();
    return 0;
}

} // namespace tt