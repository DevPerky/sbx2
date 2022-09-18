#include "AB-App-interface.h"
#include <lauxlib.h>

static AB_CreateWindow AB_CreateWindow_binding;
void AB_bind_CreateWindow(AB_CreateWindow function) {
	AB_CreateWindow_binding = function;
}

static AB_CloseWindow AB_CloseWindow_binding;
void AB_bind_CloseWindow(AB_CloseWindow function) {
	AB_CloseWindow_binding = function;
}

static AB_GetMouseXY AB_GetMouseXY_binding;
void AB_bind_GetMouseXY(AB_GetMouseXY function) {
	AB_GetMouseXY_binding = function;
}

static AB_GetWindowDimensions AB_GetWindowDimensions_binding;
void AB_bind_GetWindowDimensions(AB_GetWindowDimensions function) {
	AB_GetWindowDimensions_binding = function;
}

static AB_SetTargetFPS AB_SetTargetFPS_binding;
void AB_bind_SetTargetFPS(AB_SetTargetFPS function) {
	AB_SetTargetFPS_binding = function;
}

static AB_GetSeconds AB_GetSeconds_binding;
void AB_bind_GetSeconds(AB_GetSeconds function) {
	AB_GetSeconds_binding = function;
}


static int l_CreateWindow(lua_State *L) {
	double width;
	double height;
	const char *title;

	void *window;

	if (lua_isstring(L, -1)) {
		title = lua_tostring(L, -1);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter title. Expected type was string.");
	}

	if (lua_isnumber(L, -2)) {
		height = lua_tonumber(L, -2);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter height. Expected type was number.");
	}

	if (lua_isnumber(L, -3)) {
		width = lua_tonumber(L, -3);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter width. Expected type was number.");
	}

	if (AB_CreateWindow_binding) {
		if (!AB_CreateWindow_binding(width, height, title, &window)) {
			return luaL_error(L, "Runtime error: CreateWindow failed for unknown reason!");
		}
	}
	else {
		return luaL_error(L, "Runtime error: CreateWindow wasn't bound!");
	}

	lua_pushlightuserdata(L, window);

	return 1;
}


static int l_CloseWindow(lua_State *L) {
	void *window;


	if (lua_isuserdata(L, -1)) {
		window = lua_touserdata(L, -1);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter window. Expected type was userdata.");
	}

	if (AB_CloseWindow_binding) {
		if (!AB_CloseWindow_binding(window)) {
			return luaL_error(L, "Runtime error: CloseWindow failed for unknown reason!");
		}
	}
	else {
		return luaL_error(L, "Runtime error: CloseWindow wasn't bound!");
	}


	return 0;
}


static int l_GetMouseXY(lua_State *L) {
	void *window;

	double X;
	double Y;

	if (lua_isuserdata(L, -1)) {
		window = lua_touserdata(L, -1);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter window. Expected type was userdata.");
	}

	if (AB_GetMouseXY_binding) {
		if (!AB_GetMouseXY_binding(window, &X, &Y)) {
			return luaL_error(L, "Runtime error: GetMouseXY failed for unknown reason!");
		}
	}
	else {
		return luaL_error(L, "Runtime error: GetMouseXY wasn't bound!");
	}

	lua_pushnumber(L, X);
	lua_pushnumber(L, Y);

	return 2;
}


static int l_GetWindowDimensions(lua_State *L) {
	void *window;

	double width;
	double height;

	if (lua_isuserdata(L, -1)) {
		window = lua_touserdata(L, -1);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter window. Expected type was userdata.");
	}

	if (AB_GetWindowDimensions_binding) {
		if (!AB_GetWindowDimensions_binding(window, &width, &height)) {
			return luaL_error(L, "Runtime error: GetWindowDimensions failed for unknown reason!");
		}
	}
	else {
		return luaL_error(L, "Runtime error: GetWindowDimensions wasn't bound!");
	}

	lua_pushnumber(L, width);
	lua_pushnumber(L, height);

	return 2;
}


static int l_SetTargetFPS(lua_State *L) {
	double fps;


	if (lua_isnumber(L, -1)) {
		fps = lua_tonumber(L, -1);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter fps. Expected type was number.");
	}

	if (AB_SetTargetFPS_binding) {
		if (!AB_SetTargetFPS_binding(fps)) {
			return luaL_error(L, "Runtime error: SetTargetFPS failed for unknown reason!");
		}
	}
	else {
		return luaL_error(L, "Runtime error: SetTargetFPS wasn't bound!");
	}


	return 0;
}


static int l_GetSeconds(lua_State *L) {

	double seconds;

	if (AB_GetSeconds_binding) {
		if (!AB_GetSeconds_binding(&seconds)) {
			return luaL_error(L, "Runtime error: GetSeconds failed for unknown reason!");
		}
	}
	else {
		return luaL_error(L, "Runtime error: GetSeconds wasn't bound!");
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
