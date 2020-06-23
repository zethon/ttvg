#pragma once

namespace tt
{

class IUpdateable;
using IUpdateablePtr = std::shared_ptr<IUpdateable>;

class IUpdateable
{
public:
    virtual ~IUpdateable() = default;
    virtual std::uint16_t timestep() = 0;
};

} // namespace