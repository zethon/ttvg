#pragma once
#include <string>

#include <lua/lua.hpp>

namespace tt
{
    
void CallLuaFunction(lua_State* L, 
    const std::string& funcname, 
    const std::string& envname, 
    int objIdx);

}

template<typename ClassT>
void registerLuaFunctions(lua_State* L)
{
    luaL_newmetatable(L, ClassT::CLASS_NAME);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2); // push the metatable
    lua_settable(L, -3);  // metatable.__index = metatable

    // this creates object-like methods by populating the table
    // on the stack with the function names/pointers
    luaL_openlib(L, nullptr, ClassT::LuaMethods, 0);
        
    // clear the stack
    lua_settop(L, 0);
}

template<typename ClassT>
int registerStaticLuaType(lua_State* L, std::string_view name)
{

}