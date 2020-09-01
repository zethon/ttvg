#include <cassert>
#include <stdexcept>

#include "TTLua.h"

namespace tt
{

// no return value from the Lua function
void CallLuaFunction(lua_State* L, 
    const std::string& funcname, 
    const std::string& envname, 
    int objIdx)
{
    if (L == nullptr || funcname.size() == 0 || envname.size() == 0 || objIdx < 3)
    {
        return;
    }

    // first get the execution environment and set that
    lua_getglobal(L, envname.c_str()); // 1:env
    assert(lua_isnil(L, 1) == 0);

    // now load up the init function
    lua_getfield(L, 1, funcname.c_str()); // 1:env, 2:func
    if (lua_isnil(L, 2) != 0)
    {
        lua_settop(L, 0);
        return;
    }

    assert(lua_isfunction(L, 2) == 1);

    // now get the parameter we're passing to Lua which is a Scene* (aka `this`)
    lua_rawgeti(L, LUA_REGISTRYINDEX, objIdx); // 1:env, 2:func, 1:ud
    if (lua_pcall(L, 1, 0, 0) != 0) // 1:env, 2:retval
    {
        auto error = lua_tostring(L, -1);
        throw std::runtime_error(error);
    }

    lua_settop(L, 0);
}

}