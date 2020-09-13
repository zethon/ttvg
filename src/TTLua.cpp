#include <cassert>
#include <stdexcept>
#include <variant>
#include <vector>

#include "TTLua.h"

namespace tt
{

template<>
bool GetLuaValue(const LuaArgPair& v)
{
    const auto& [type, value] = v;
    if (type != LUA_TBOOLEAN)
    {
        throw std::runtime_error("Lua valid is not a boolean");
    }
    return std::any_cast<bool>(value);
}

template<>
float GetLuaValue(const LuaArgPair& v)
{
    const auto& [type, value] = v;
    if (type != LUA_TNUMBER)
    {
        throw std::runtime_error("Lua valid is not a number");
    }
    return std::any_cast<float>(value);
}

template<>
std::string GetLuaValue(const LuaArgPair& v)
{
    const auto& [type, value] = v;
    if (type != LUA_TSTRING)
    {
        throw std::runtime_error("Lua valid is not a string");
    }
    return std::any_cast<std::string>(value);
}

[[maybe_unused]] OptionalLuaValues CallLuaFunction(lua_State* L,
    std::string_view function,
    std::string_view sandbox)
{
    return CallLuaFunction(L, function, sandbox, LuaValues{});
}

[[maybe_unused]] OptionalLuaValues CallLuaFunction(lua_State* L,
    std::string_view function,
    std::string_view sandbox,
    const LuaArgPair& arg)
{
    LuaValues args;
    args.push_back(arg);
    return CallLuaFunction(L, function, sandbox, args);
}

[[maybe_unused]] OptionalLuaValues CallLuaFunction(lua_State * L, 
    std::string_view function, 
    std::string_view sandbox,
    const LuaValues & args)
{
    if (L == nullptr || function.size() == 0)
    {
        return {};
    }
    
    if (sandbox.size() > 0)
    {
        // first get the execution environment and set that
        lua_getglobal(L, sandbox.data()); // 1:env
        assert(lua_isnil(L, 1) == 0);
    }

    // save off the stack so we can get an accurate
    int stack_size = lua_gettop(L);

    // now load up the function
    lua_getfield(L, 1, function.data()); // 1:env, 2:func
    if (lua_isnil(L, 2) != 0
        || lua_isfunction(L, 2) == 0)
    {
        lua_settop(L, 0);
        return {};
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

    if (lua_pcall(L, argcount, LUA_MULTRET, 0) != 0)
    {
        const auto error = fmt::format("error running callback {}: {}",
            function, lua_tostring(L, -1));
        std::cout << error << '\n';
        throw std::runtime_error(error);
    }

    LuaValues retval;
    int num_returns = lua_gettop(L) - stack_size;

    for (int i = 0; i < num_returns; i++)
    {
        int stack_idx = (i - num_returns);
        int type = lua_type(L, stack_idx);
        switch (type)
        {
            default:
                throw std::runtime_error("unsupported return type");

            case LUA_TBOOLEAN:
            {
                bool result = lua_toboolean(L, stack_idx) == 0 ? false : true;
                retval.push_back({ LUA_TBOOLEAN, result });
            }
            break;

            case LUA_TNUMBER:
            {
                float result = static_cast<float>(lua_tonumber(L, stack_idx));
                retval.push_back({ LUA_TNUMBER, result });
            }
            break;

            case LUA_TSTRING:
            {
                const std::string result = lua_tostring(L, stack_idx);
                retval.push_back({ LUA_TSTRING, result });
            }
            break;
        }
    }

    // since we're processing return values, we can clean
    // up the stack before we leave
    lua_settop(L, 0);

    return retval;
}

}