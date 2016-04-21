#ifndef INCLUDE_CLIENTSCRIPTING
#define INCLUDE_CLIENTSCRIPTING

#include <lua/lua.hpp>

namespace Lua
{
    extern lua_State * LuaHUD;
    
    int tolua_print (lua_State * L);
    
    bool do_hud();
    
    void scripting_init();
}

#endif
