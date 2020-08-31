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