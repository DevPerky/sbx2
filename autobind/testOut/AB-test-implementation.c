#include "AB-test-interface.h"
#include <lauxlib.h>

static AB_customTypeIn AB_customTypeIn_binding;

static Test AB_to_Test(lua_State *L, int index);
static Test2 AB_to_Test2(lua_State *L, int index);
static TestIntegers AB_to_TestIntegers(lua_State *L, int index);
int AB_push_Test(lua_State *L, Test value);
int AB_push_Test2(lua_State *L, Test2 value);
int AB_push_TestIntegers(lua_State *L, TestIntegers value);

static Test AB_to_Test(lua_State *L, int index) {
	Test result;

	lua_getfield(L, index, "testNumber");
	result.testNumber = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, index, "testString");
	result.testString = lua_tostring(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, index, "testUserData");
	result.testUserData = lua_touserdata(L, -1);
	lua_pop(L, 1);

	return result;
}

static Test2 AB_to_Test2(lua_State *L, int index) {
	Test2 result;

	lua_getfield(L, index, "customParam");
	result.customParam = AB_to_Test(L, -1);
	lua_pop(L, 1);

	return result;
}

static TestIntegers AB_to_TestIntegers(lua_State *L, int index) {
	TestIntegers result;

	lua_getfield(L, index, "integer");
	result.integer = lua_tointeger(L, -1);
	lua_pop(L, 1);

	return result;
}

int AB_push_Test(lua_State *L, Test value) {
	lua_createtable(L, 0, 3);

	lua_pushstring(L, "testNumber");
	lua_pushnumber(L, value.testNumber);
	lua_settable(L, -3);

	lua_pushstring(L, "testString");
	lua_pushstring(L, value.testString);
	lua_settable(L, -3);

	lua_pushstring(L, "testUserData");
	lua_pushlightuserdata(L, value.testUserData);
	lua_settable(L, -3);
}

int AB_push_Test2(lua_State *L, Test2 value) {
	lua_createtable(L, 0, 1);

	lua_pushstring(L, "customParam");
	AB_push_Test(L, value.customParam);
	lua_settable(L, -3);
}

int AB_push_TestIntegers(lua_State *L, TestIntegers value) {
	lua_createtable(L, 0, 1);

	lua_pushstring(L, "integer");
	lua_pushinteger(L, value.integer);
	lua_settable(L, -3);
}

void AB_bind_customTypeIn(AB_customTypeIn function) {
	AB_customTypeIn_binding = function;
}

static int l_customTypeIn(lua_State *L) {
	Test customParam;
	const char *stringParam;
	const char *outString;
	Test customParamOut;

	if(lua_isstring(L, -1)) {
		stringParam = lua_tostring(L, -1);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter stringParam! Expected type was string");
	}

	if(lua_istable(L, -2)) {
		customParam = AB_to_Test(L, -2);
	}
	else {
		return luaL_error(L, " Error: Wrong type of parameter customParam! Expected type was table");
	}

	if(AB_customTypeIn_binding != 0) {
		if(AB_customTypeIn_binding(customParam, stringParam, &outString, &customParamOut) == 0) {
			luaL_error(L, "Runtime error: customTypeIn failed for some reason.");
		}
	}

	lua_pushstring(L, outString);
	AB_push_Test(L, customParamOut);
	return 2;
}

void AB_registerModule_test(lua_State *L) {
	lua_register(L, "customTypeIn", l_customTypeIn);
}