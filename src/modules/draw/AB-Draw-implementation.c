#include "AB-Draw-interface.h"
#include <lauxlib.h>

static AB_SetViewport AB_SetViewport_binding;
static AB_CreateDrawMatrix AB_CreateDrawMatrix_binding;
static AB_SetOrtho AB_SetOrtho_binding;
static AB_SetProjectionMatrix AB_SetProjectionMatrix_binding;
static AB_SetDrawColor AB_SetDrawColor_binding;
static AB_DrawRectangle AB_DrawRectangle_binding;
static AB_DrawLine AB_DrawLine_binding;
static AB_Clear AB_Clear_binding;

static Vector2 AB_to_Vector2(lua_State *L, int index);
static Rectangle AB_to_Rectangle(lua_State *L, int index);
static Camera AB_to_Camera(lua_State *L, int index);
int AB_push_Vector2(lua_State *L, Vector2 value);
int AB_push_Rectangle(lua_State *L, Rectangle value);
int AB_push_Camera(lua_State *L, Camera value);

static Vector2 AB_to_Vector2(lua_State *L, int index) {
	Vector2 result;

	lua_getfield(L, index, "x");
	result.x = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, index, "y");
	result.y = lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

static Rectangle AB_to_Rectangle(lua_State *L, int index) {
	Rectangle result;

	lua_getfield(L, index, "left");
	result.left = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, index, "top");
	result.top = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, index, "right");
	result.right = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, index, "bottom");
	result.bottom = lua_tonumber(L, -1);
	lua_pop(L, 1);

	return result;
}

static Camera AB_to_Camera(lua_State *L, int index) {
	Camera result;

	lua_getfield(L, index, "position");
	result.position = AB_to_Vector2(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, index, "viewPort");
	result.viewPort = AB_to_Rectangle(L, -1);
	lua_pop(L, 1);

	return result;
}

int AB_push_Vector2(lua_State *L, Vector2 value) {
	lua_createtable(L, 0, 2);

	lua_pushstring(L, "x");
	lua_pushnumber(L, value.x);
	lua_settable(L, -3);

	lua_pushstring(L, "y");
	lua_pushnumber(L, value.y);
	lua_settable(L, -3);
}

int AB_push_Rectangle(lua_State *L, Rectangle value) {
	lua_createtable(L, 0, 4);

	lua_pushstring(L, "left");
	lua_pushnumber(L, value.left);
	lua_settable(L, -3);

	lua_pushstring(L, "top");
	lua_pushnumber(L, value.top);
	lua_settable(L, -3);

	lua_pushstring(L, "right");
	lua_pushnumber(L, value.right);
	lua_settable(L, -3);

	lua_pushstring(L, "bottom");
	lua_pushnumber(L, value.bottom);
	lua_settable(L, -3);
}

int AB_push_Camera(lua_State *L, Camera value) {
	lua_createtable(L, 0, 2);

	lua_pushstring(L, "position");
	AB_push_Vector2(L, value.position);
	lua_settable(L, -3);

	lua_pushstring(L, "viewPort");
	AB_push_Rectangle(L, value.viewPort);
	lua_settable(L, -3);
}

void AB_bind_SetViewport(AB_SetViewport function) {
	AB_SetViewport_binding = function;
}

void AB_bind_CreateDrawMatrix(AB_CreateDrawMatrix function) {
	AB_CreateDrawMatrix_binding = function;
}

void AB_bind_SetOrtho(AB_SetOrtho function) {
	AB_SetOrtho_binding = function;
}

void AB_bind_SetProjectionMatrix(AB_SetProjectionMatrix function) {
	AB_SetProjectionMatrix_binding = function;
}

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

static int l_SetViewport(lua_State *L) {
	Rectangle bounds;

	if(lua_istable(L, -1)) {
		bounds = AB_to_Rectangle(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter bounds! Expected type was table");
	}

	if(AB_SetViewport_binding != 0) {
		if(AB_SetViewport_binding(bounds) == 0) {
			luaL_error(L, "Runtime error: SetViewport failed for some reason.");
		}
	}

	return 0;
}

static int l_CreateDrawMatrix(lua_State *L) {
	lua_Integer handle;

	if(AB_CreateDrawMatrix_binding != 0) {
		if(AB_CreateDrawMatrix_binding(&handle) == 0) {
			luaL_error(L, "Runtime error: CreateDrawMatrix failed for some reason.");
		}
	}

	lua_pushinteger(L, handle);
	return 1;
}

static int l_SetOrtho(lua_State *L) {
	lua_Integer matrixHandle;
	Rectangle bounds;

	if(lua_istable(L, -1)) {
		bounds = AB_to_Rectangle(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter bounds! Expected type was table");
	}

	if(lua_isinteger(L, -2)) {
		matrixHandle = lua_tointeger(L, -2);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter matrixHandle! Expected type was integer");
	}

	if(AB_SetOrtho_binding != 0) {
		if(AB_SetOrtho_binding(matrixHandle, bounds) == 0) {
			luaL_error(L, "Runtime error: SetOrtho failed for some reason.");
		}
	}

	return 0;
}

static int l_SetProjectionMatrix(lua_State *L) {
	lua_Integer matrixHandle;

	if(lua_isinteger(L, -1)) {
		matrixHandle = lua_tointeger(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter matrixHandle! Expected type was integer");
	}

	if(AB_SetProjectionMatrix_binding != 0) {
		if(AB_SetProjectionMatrix_binding(matrixHandle) == 0) {
			luaL_error(L, "Runtime error: SetProjectionMatrix failed for some reason.");
		}
	}

	return 0;
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
	lua_register(L, "SetViewport", l_SetViewport);
	lua_register(L, "CreateDrawMatrix", l_CreateDrawMatrix);
	lua_register(L, "SetOrtho", l_SetOrtho);
	lua_register(L, "SetProjectionMatrix", l_SetProjectionMatrix);
	lua_register(L, "SetDrawColor", l_SetDrawColor);
	lua_register(L, "DrawRectangle", l_DrawRectangle);
	lua_register(L, "DrawLine", l_DrawLine);
	lua_register(L, "Clear", l_Clear);
}