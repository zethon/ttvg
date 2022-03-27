#pragma once

namespace tt
{

enum Direction
{
    NONE    = 0x00,
    UP      = 0x01,
    DOWN    = 0x02,
    LEFT    = 0x04,
    RIGHT   = 0x08
};

inline std::string DirectionToString(tt::Direction d)
{
    switch (d)
    {
        case tt::Direction::UP: return "up"; break;
        case tt::Direction::DOWN: return "down"; break;
        case tt::Direction::LEFT: return "left"; break;
        case tt::Direction::RIGHT: return "right"; break;
    }

    return {};
}

} // namespace
