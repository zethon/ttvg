#include "Background.h"

namespace tt
{

Background::Background(sf::Texture texture)
        : _texture { std::move(texture) }
{
    setTexture(_texture);
}

} // namespace tt