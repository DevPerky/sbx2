#include "AB-Draw-interface.h"
#include <lauxlib.h>

static AB_SetDrawColor AB_SetDrawColor_binding;
static AB_DrawRectangle AB_DrawRectangle_binding;
static AB_DrawLine AB_DrawLine_binding;
static AB_Clear AB_Clear_binding;


void AB_bind_SetDrawColor(AB_SetDrawColor function) {
	AB_SetDrawColor_binding = function;
}

void AB_bind_DrawRectangle(AB_DrawRectangle function) {
	AB_DrawRectangle_binding = function;
}

void AB_bind_DrawLine(AB_DrawLine function) {
	AB_DrawLine_binding = function;
}

void AB_bind_Clear(AB_Clear function) {
	AB_Clear_binding = function;
}

static int l_SetDrawColor(lua_State *L) {
	double red;
	double green;
	double blue;
	double alpha;

	if(lua_isnumber(L, -1)) {
		alpha = lua_tonumber(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter alpha! Expected type was number");
	}

	if(lua_isnumber(L, -2)) {
		blue = lua_tonumber(L, -2);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter blue! Expected type was number");
	}

	if(lua_isnumber(L, -3)) {
		green = lua_tonumber(L, -3);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter green! Expected type was number");
	}

	if(lua_isnumber(L, -4)) {
		red = lua_tonumber(L, -4);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter red! Expected type was number");
	}

	if(AB_SetDrawColor_binding != 0) {
		if(AB_SetDrawColor_binding(red, green, blue, alpha) == 0) {
			luaL_error(L, "Runtime error: SetDrawColor failed for some reason.");
		}
	}

	return 0;
}

static int l_DrawRectangle(lua_State *L) {
	double left;
	double top;
	double right;
	double bottom;

	if(lua_isnumber(L, -1)) {
		bottom = lua_tonumber(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter bottom! Expected type was number");
	}

	if(lua_isnumber(L, -2)) {
		right = lua_tonumber(L, -2);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter right! Expected type was number");
	}

	if(lua_isnumber(L, -3)) {
		top = lua_tonumber(L, -3);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter top! Expected type was number");
	}

	if(lua_isnumber(L, -4)) {
		left = lua_tonumber(L, -4);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter left! Expected type was number");
	}

	if(AB_DrawRectangle_binding != 0) {
		if(AB_DrawRectangle_binding(left, top, right, bottom) == 0) {
			luaL_error(L, "Runtime error: DrawRectangle failed for some reason.");
		}
	}

	return 0;
}

static int l_DrawLine(lua_State *L) {
	double fromX;
	double fromY;
	double toX;
	double toY;
	double width;

	if(lua_isnumber(L, -1)) {
		width = lua_tonumber(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter width! Expected type was number");
	}

	if(lua_isnumber(L, -2)) {
		toY = lua_tonumber(L, -2);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter toY! Expected type was number");
	}

	if(lua_isnumber(L, -3)) {
		toX = lua_tonumber(L, -3);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter toX! Expected type was number");
	}

	if(lua_isnumber(L, -4)) {
		fromY = lua_tonumber(L, -4);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter fromY! Expected type was number");
	}

	if(lua_isnumber(L, -5)) {
		fromX = lua_tonumber(L, -5);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter fromX! Expected type was number");
	}

	if(AB_DrawLine_binding != 0) {
		if(AB_DrawLine_binding(fromX, fromY, toX, toY, width) == 0) {
			luaL_error(L, "Runtime error: DrawLine failed for some reason.");
		}
	}

	return 0;
}

static int l_Clear(lua_State *L) {
	double red;
	double green;
	double blue;
	double alpha;

	if(lua_isnumber(L, -1)) {
		alpha = lua_tonumber(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter alpha! Expected type was number");
	}

	if(lua_isnumber(L, -2)) {
		blue = lua_tonumber(L, -2);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter blue! Expected type was number");
	}

	if(lua_isnumber(L, -3)) {
		green = lua_tonumber(L, -3);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter green! Expected type was number");
	}

	if(lua_isnumber(L, -4)) {
		red = lua_tonumber(L, -4);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter red! Expected type was number");
	}

	if(AB_Clear_binding != 0) {
		if(AB_Clear_binding(red, green, blue, alpha) == 0) {
			luaL_error(L, "Runtime error: Clear failed for some reason.");
		}
	}

	return 0;
}

void AB_registerModule_Draw(lua_State *L) {
	lua_register(L, "SetDrawColor", l_SetDrawColor);
	lua_register(L, "DrawRectangle", l_DrawRectangle);
	lua_register(L, "DrawLine", l_DrawLine);
	lua_register(L, "Clear", l_Clear);
}