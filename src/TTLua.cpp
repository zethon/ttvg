#include <cassert>
#include <stdexcept>
#include <variant>
#include <vector>

#include "TTLua.h"

namespace tt
{

void CallLuaFunction(lua_State * L, 
    std::string_view function, 
    std::string_view sandbox,
    const LuaArguments & args,
    bool clearRetVals)
{
    if (L == nullptr || function.size() == 0)
    {
        return;
    }

    if (sandbox.size() > 0)
    {
        // first get the execution environment and set that
        lua_getglobal(L, sandbox.data()); // 1:env
        assert(lua_isnil(L, 1) == 0);
    }

    // now load up the function
    lua_getfield(L, 1, function.data()); // 1:env, 2:func
    if (lua_isnil(L, 2) != 0
        || lua_isfunction(L, 2) == 0)
    {
        lua_settop(L, 0);
        return;
    }

    std::uint32_t argcount = 0;
    for (const auto& [type, value] : args)
    {
        switch (type)
        {
            default:
                throw std::runtime_error("unsupported parameter type");

            case LUA_TBOOLEAN:
                lua_pushboolean(L, std::any_cast<bool>(value) ? 1 : 0);
                argcount++;
            break;

            case LUA_TLIGHTUSERDATA:
                lua_pushlightuserdata(L, std::any_cast<void*>(value));
                argcount++;
            break;

            case LUA_TNUMBER:
                lua_pushnumber(L, std::any_cast<lua_Number>(value));
                argcount++;
            break;

            case LUA_TSTRING:
                lua_pushstring(L, std::any_cast<std::string>(value).c_str());
                argcount++;
            break;

            case LUA_REGISTRYINDEX:
                lua_rawgeti(L, LUA_REGISTRYINDEX, std::any_cast<int>(value));
                argcount++;
            break;
        }
    }

    if (lua_pcall(L, argcount, LUA_MULTRET, 0) != 0) // 1:env, 2:retval
    {
        auto error = lua_tostring(L, -1);
        throw std::runtime_error(error);
    }

    if (clearRetVals)
    {
        lua_settop(L, 0);
    }
}

}