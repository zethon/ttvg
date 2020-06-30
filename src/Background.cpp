#include "Background.h"

namespace tt
{

Background::Background(sf::Texture texture)
        : _texture { std::move(texture) }
{
    setTexture(_texture);
}

sf::FloatRect Background::getWorldRect() const
{
    return { 0, 0, getRightBoundary(), getBottomBoundary() };
}

float Background::getRightBoundary() const
{
    return getTextureRect().width * this->getScale().x;
}

float Background::getBottomBoundary() const
{
    return getTextureRect().height * this->getScale().y;;
}

} // namespace tt