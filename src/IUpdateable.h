#pragma once
#include <memory>

namespace tt
{

class IUpdateable;
using IUpdateablePtr = std::shared_ptr<IUpdateable>;

class IUpdateable
{
public:
    virtual ~IUpdateable() = default;
    virtual std::uint16_t update() = 0;
};

} // namespace