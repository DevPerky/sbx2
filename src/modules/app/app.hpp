#ifndef APP_H
#define APP_H
#include <lua.hpp>

void appInitialize(lua_State *L);
void appLoop(lua_State *L);

#endif