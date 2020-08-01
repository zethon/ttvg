#pragma once

namespace tt
{

enum class Scenes : std::size_t
{
    TUCSON = 0,
    EUCLIDHOUSE
};

enum Direction
{
    NONE    = 0x00,
    UP      = 0x01,
    DOWN    = 0x02,
    LEFT    = 0x04,
    RIGHT   = 0x08
};

enum AnimatedState
{
    STILL,
    ANIMATED
};

} // namespace
