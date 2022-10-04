#include <lua.hpp>
#include <thread>
#include "modules/app/app.hpp"
#include "modules/draw/draw.hpp"

extern "C" {
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <lauxlib.h>

}



int main() {
	if (glfwInit() != GLFW_TRUE) {

		return 1;
	}

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	appInitialize(L);
	drawInitialize(L);

	luaL_dofile(L, "test.lua");
	appLoop(L);
	
    return 0;
}