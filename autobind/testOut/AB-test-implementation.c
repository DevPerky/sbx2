#include "AB-test-interface.h"
#include <lauxlib.h>

static AB_threeInThreeOut AB_threeInThreeOut_binding;
void AB_bind_threeInThreeOut(AB_threeInThreeOut function) {
	AB_threeInThreeOut_binding = function;
}

static AB_theeInZeroOut AB_theeInZeroOut_binding;
void AB_bind_theeInZeroOut(AB_theeInZeroOut function) {
	AB_theeInZeroOut_binding = function;
}

static AB_zeroInThreeOut AB_zeroInThreeOut_binding;
void AB_bind_zeroInThreeOut(AB_zeroInThreeOut function) {
	AB_zeroInThreeOut_binding = function;
}


static int l_threeInThreeOut(lua_State *L) {
	double in_param1;
	const char *in_param2;
	void *in_param3;

	double out_param;
	const char *out_param1;
	void *out_param2;

	if (lua_isuserdata(L, -1)) {
		in_param3 = lua_touserdata(L, -1);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter in_param3. Expected type was userdata.");
	}

	if (lua_isstring(L, -2)) {
		in_param2 = lua_tostring(L, -2);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter in_param2. Expected type was string.");
	}

	if (lua_isnumber(L, -3)) {
		in_param1 = lua_tonumber(L, -3);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter in_param1. Expected type was number.");
	}

	if (AB_threeInThreeOut_binding) {
		if (!AB_threeInThreeOut_binding(in_param1, in_param2, in_param3, &out_param, &out_param1, &out_param2)) {
			return luaL_error(L, "Runtime error: threeInThreeOut failed for unknown reason!");
		}
	}
	else {
		return luaL_error(L, "Runtime error: threeInThreeOut wasn't bound!");
	}

	lua_pushnumber(L, out_param);
	lua_pushstring(L, out_param1);
	lua_pushlightuserdata(L, out_param2);

	return 3;
}


static int l_theeInZeroOut(lua_State *L) {
	double out_param;
	const char *out_param1;
	void *out_param2;


	if (lua_isuserdata(L, -1)) {
		out_param2 = lua_touserdata(L, -1);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter out_param2. Expected type was userdata.");
	}

	if (lua_isstring(L, -2)) {
		out_param1 = lua_tostring(L, -2);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter out_param1. Expected type was string.");
	}

	if (lua_isnumber(L, -3)) {
		out_param = lua_tonumber(L, -3);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter out_param. Expected type was number.");
	}

	if (AB_theeInZeroOut_binding) {
		if (!AB_theeInZeroOut_binding(out_param, out_param1, out_param2)) {
			return luaL_error(L, "Runtime error: theeInZeroOut failed for unknown reason!");
		}
	}
	else {
		return luaL_error(L, "Runtime error: theeInZeroOut wasn't bound!");
	}


	return 0;
}


static int l_zeroInThreeOut(lua_State *L) {

	double out_param;
	const char *out_param1;
	void *out_param2;

	if (AB_zeroInThreeOut_binding) {
		if (!AB_zeroInThreeOut_binding(&out_param, &out_param1, &out_param2)) {
			return luaL_error(L, "Runtime error: zeroInThreeOut failed for unknown reason!");
		}
	}
	else {
		return luaL_error(L, "Runtime error: zeroInThreeOut wasn't bound!");
	}

	lua_pushnumber(L, out_param);
	lua_pushstring(L, out_param1);
	lua_pushlightuserdata(L, out_param2);

	return 3;
}


void AB_registerModule_test(lua_State *L) {
	lua_register(L, "threeInThreeOut", l_threeInThreeOut);
	lua_register(L, "theeInZeroOut", l_theeInZeroOut);
	lua_register(L, "zeroInThreeOut", l_zeroInThreeOut);
}
