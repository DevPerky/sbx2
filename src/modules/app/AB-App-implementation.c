#include "AB-App-interface.h"
#include <lauxlib.h>

static AB_CreateWindow AB_CreateWindow_binding;
static AB_CloseWindow AB_CloseWindow_binding;
static AB_GetMouseXY AB_GetMouseXY_binding;
static AB_GetWindowDimensions AB_GetWindowDimensions_binding;
static AB_SetTargetFPS AB_SetTargetFPS_binding;
static AB_GetSeconds AB_GetSeconds_binding;


void AB_bind_CreateWindow(AB_CreateWindow function) {
	AB_CreateWindow_binding = function;
}

void AB_bind_CloseWindow(AB_CloseWindow function) {
	AB_CloseWindow_binding = function;
}

void AB_bind_GetMouseXY(AB_GetMouseXY function) {
	AB_GetMouseXY_binding = function;
}

void AB_bind_GetWindowDimensions(AB_GetWindowDimensions function) {
	AB_GetWindowDimensions_binding = function;
}

void AB_bind_SetTargetFPS(AB_SetTargetFPS function) {
	AB_SetTargetFPS_binding = function;
}

void AB_bind_GetSeconds(AB_GetSeconds function) {
	AB_GetSeconds_binding = function;
}

static int l_CreateWindow(lua_State *L) {
	double width;
	double height;
	const char *title;
	void *window;

	if(lua_isstring(L, -1)) {
		title = lua_tostring(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter title! Expected type was string");
	}

	if(lua_isnumber(L, -2)) {
		height = lua_tonumber(L, -2);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter height! Expected type was number");
	}

	if(lua_isnumber(L, -3)) {
		width = lua_tonumber(L, -3);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter width! Expected type was number");
	}

	if(AB_CreateWindow_binding != 0) {
		if(AB_CreateWindow_binding(width, height, title, &window) == 0) {
			luaL_error(L, "Runtime error: CreateWindow failed for some reason.");
		}
	}

	lua_pushlightuserdata(L, window);
	return 1;
}

static int l_CloseWindow(lua_State *L) {
	void *window;

	if(lua_islightuserdata(L, -1)) {
		window = lua_touserdata(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter window! Expected type was userdata");
	}

	if(AB_CloseWindow_binding != 0) {
		if(AB_CloseWindow_binding(window) == 0) {
			luaL_error(L, "Runtime error: CloseWindow failed for some reason.");
		}
	}

	return 0;
}

static int l_GetMouseXY(lua_State *L) {
	void *window;
	double X;
	double Y;

	if(lua_islightuserdata(L, -1)) {
		window = lua_touserdata(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter window! Expected type was userdata");
	}

	if(AB_GetMouseXY_binding != 0) {
		if(AB_GetMouseXY_binding(window, &X, &Y) == 0) {
			luaL_error(L, "Runtime error: GetMouseXY failed for some reason.");
		}
	}

	lua_pushnumber(L, X);
	lua_pushnumber(L, Y);
	return 2;
}

static int l_GetWindowDimensions(lua_State *L) {
	void *window;
	double width;
	double height;

	if(lua_islightuserdata(L, -1)) {
		window = lua_touserdata(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter window! Expected type was userdata");
	}

	if(AB_GetWindowDimensions_binding != 0) {
		if(AB_GetWindowDimensions_binding(window, &width, &height) == 0) {
			luaL_error(L, "Runtime error: GetWindowDimensions failed for some reason.");
		}
	}

	lua_pushnumber(L, width);
	lua_pushnumber(L, height);
	return 2;
}

static int l_SetTargetFPS(lua_State *L) {
	double fps;

	if(lua_isnumber(L, -1)) {
		fps = lua_tonumber(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter fps! Expected type was number");
	}

	if(AB_SetTargetFPS_binding != 0) {
		if(AB_SetTargetFPS_binding(fps) == 0) {
			luaL_error(L, "Runtime error: SetTargetFPS failed for some reason.");
		}
	}

	return 0;
}

static int l_GetSeconds(lua_State *L) {
	double seconds;

	if(AB_GetSeconds_binding != 0) {
		if(AB_GetSeconds_binding(&seconds) == 0) {
			luaL_error(L, "Runtime error: GetSeconds failed for some reason.");
		}
	}

	lua_pushnumber(L, seconds);
	return 1;
}

void AB_registerModule_App(lua_State *L) {
	lua_register(L, "CreateWindow", l_CreateWindow);
	lua_register(L, "CloseWindow", l_CloseWindow);
	lua_register(L, "GetMouseXY", l_GetMouseXY);
	lua_register(L, "GetWindowDimensions", l_GetWindowDimensions);
	lua_register(L, "SetTargetFPS", l_SetTargetFPS);
	lua_register(L, "GetSeconds", l_GetSeconds);
}