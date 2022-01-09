#pragma once
#include <memory>

#include <spdlog/spdlog.h>

#include <lua.hpp>

namespace tt
{

namespace log
{

constexpr auto GLOBAL_LOGGER = "tt";

using SpdLogPtr = std::shared_ptr<spdlog::logger>;

[[maybe_unused]] SpdLogPtr rootLogger();
SpdLogPtr initializeLogger(const std::string& name);

} // namespace log

namespace
{

int Logger_trace(lua_State* L)
{
    auto logger = log::initializeLogger("LuaScript");
    logger->trace(lua_tostring(L, 1));
    return 0;
}


int Logger_debug(lua_State* L)
{
    auto logger = log::initializeLogger("LuaScript");
    logger->debug(lua_tostring(L, 1));
    return 0;
}


int Logger_info(lua_State* L)
{
    auto logger = log::initializeLogger("LuaScript");
    logger->info(lua_tostring(L, 1));
    return 0;
}

int Logger_warning(lua_State* L)
{
    auto logger = log::initializeLogger("LuaScript");
    logger->warn(lua_tostring(L, 1));
    return 0;
}

int Logger_error(lua_State* L)
{
    auto logger = log::initializeLogger("LuaScript");
    logger->error(lua_tostring(L, 1));
    return 0;
}

int Logger_critical(lua_State* L)
{
    auto logger = log::initializeLogger("LuaScript");
    logger->critical(lua_tostring(L, 1));
    return 0;
}

} // namespace

const struct luaL_Reg Logger_LuaMethods[] =
{
    {"trace", Logger_trace},
    {"debug", Logger_debug},
    {"info", Logger_info},
    {"warn", Logger_warning},
    {"error", Logger_error},
    {"critical", Logger_critical},
    {nullptr, nullptr}
};


} // namespace tt