#include "AB-Draw-interface.h"
#include <lauxlib.h>

static AB_DrawSetViewport AB_DrawSetViewport_binding;
static AB_MatrixNew AB_MatrixNew_binding;
static AB_MatrixSetOrtho AB_MatrixSetOrtho_binding;
static AB_DrawCircle AB_DrawCircle_binding;
static AB_DrawSetProjectionMatrix AB_DrawSetProjectionMatrix_binding;
static AB_DrawSetColor AB_DrawSetColor_binding;
static AB_DrawRectangle AB_DrawRectangle_binding;
static AB_DrawLine AB_DrawLine_binding;
static AB_DrawClear AB_DrawClear_binding;

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

void AB_bind_DrawSetViewport(AB_DrawSetViewport function) {
	AB_DrawSetViewport_binding = function;
}

void AB_bind_MatrixNew(AB_MatrixNew function) {
	AB_MatrixNew_binding = function;
}

void AB_bind_MatrixSetOrtho(AB_MatrixSetOrtho function) {
	AB_MatrixSetOrtho_binding = function;
}

void AB_bind_DrawCircle(AB_DrawCircle function) {
	AB_DrawCircle_binding = function;
}

void AB_bind_DrawSetProjectionMatrix(AB_DrawSetProjectionMatrix function) {
	AB_DrawSetProjectionMatrix_binding = function;
}

void AB_bind_DrawSetColor(AB_DrawSetColor function) {
	AB_DrawSetColor_binding = function;
}

void AB_bind_DrawRectangle(AB_DrawRectangle function) {
	AB_DrawRectangle_binding = function;
}

void AB_bind_DrawLine(AB_DrawLine function) {
	AB_DrawLine_binding = function;
}

void AB_bind_DrawClear(AB_DrawClear function) {
	AB_DrawClear_binding = function;
}

static int l_DrawSetViewport(lua_State *L) {
	Rectangle bounds;

	if(lua_istable(L, -1)) {
		bounds = AB_to_Rectangle(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter bounds! Expected type was table");
	}

	if(AB_DrawSetViewport_binding != 0) {
		if(AB_DrawSetViewport_binding(bounds) == 0) {
			luaL_error(L, "Runtime error: DrawSetViewport failed for some reason.");
		}
	}

	return 0;
}

static int l_MatrixNew(lua_State *L) {
	lua_Integer handle;

	if(AB_MatrixNew_binding != 0) {
		if(AB_MatrixNew_binding(&handle) == 0) {
			luaL_error(L, "Runtime error: MatrixNew failed for some reason.");
		}
	}

	lua_pushinteger(L, handle);
	return 1;
}

static int l_MatrixSetOrtho(lua_State *L) {
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

	if(AB_MatrixSetOrtho_binding != 0) {
		if(AB_MatrixSetOrtho_binding(matrixHandle, bounds) == 0) {
			luaL_error(L, "Runtime error: MatrixSetOrtho failed for some reason.");
		}
	}

	return 0;
}

static int l_DrawCircle(lua_State *L) {
	Vector2 position;
	double radius;

	if(lua_isnumber(L, -1)) {
		radius = lua_tonumber(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter radius! Expected type was number");
	}

	if(lua_istable(L, -2)) {
		position = AB_to_Vector2(L, -2);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter position! Expected type was table");
	}

	if(AB_DrawCircle_binding != 0) {
		if(AB_DrawCircle_binding(position, radius) == 0) {
			luaL_error(L, "Runtime error: DrawCircle failed for some reason.");
		}
	}

	return 0;
}

static int l_DrawSetProjectionMatrix(lua_State *L) {
	lua_Integer matrixHandle;

	if(lua_isinteger(L, -1)) {
		matrixHandle = lua_tointeger(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter matrixHandle! Expected type was integer");
	}

	if(AB_DrawSetProjectionMatrix_binding != 0) {
		if(AB_DrawSetProjectionMatrix_binding(matrixHandle) == 0) {
			luaL_error(L, "Runtime error: DrawSetProjectionMatrix failed for some reason.");
		}
	}

	return 0;
}

static int l_DrawSetColor(lua_State *L) {
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

	if(AB_DrawSetColor_binding != 0) {
		if(AB_DrawSetColor_binding(red, green, blue, alpha) == 0) {
			luaL_error(L, "Runtime error: DrawSetColor failed for some reason.");
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

static int l_DrawClear(lua_State *L) {
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

	if(AB_DrawClear_binding != 0) {
		if(AB_DrawClear_binding(red, green, blue, alpha) == 0) {
			luaL_error(L, "Runtime error: DrawClear failed for some reason.");
		}
	}

	return 0;
}

void AB_registerModule_Draw(lua_State *L) {
	lua_register(L, "DrawSetViewport", l_DrawSetViewport);
	lua_register(L, "MatrixNew", l_MatrixNew);
	lua_register(L, "MatrixSetOrtho", l_MatrixSetOrtho);
	lua_register(L, "DrawCircle", l_DrawCircle);
	lua_register(L, "DrawSetProjectionMatrix", l_DrawSetProjectionMatrix);
	lua_register(L, "DrawSetColor", l_DrawSetColor);
	lua_register(L, "DrawRectangle", l_DrawRectangle);
	lua_register(L, "DrawLine", l_DrawLine);
	lua_register(L, "DrawClear", l_DrawClear);
}