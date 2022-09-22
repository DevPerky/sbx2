#include "AB-test-interface.h"
#include <lauxlib.h>

int AB_toTest(lua_State *L, int index, Test *to);
int AB_toTest2(lua_State *L, int index, Test2 *to);

int AB_toTest(lua_State *L, int index, Test *to) {
	double testNumber;
	const char *testString;
	void *testUserData;
	lua_getfield(L, index, "testNumber");
	if (lua_isnumber(L, 1)) {
		testNumber = lua_tonumber(L, 1);
	}
	else {
		lua_pop(L, 1);
		return 0;
	}
	to->testNumber = testNumber;
	lua_pop(L, 1);

	lua_getfield(L, index, "testString");
	if (lua_isstring(L, 1)) {
		testString = lua_tostring(L, 1);
	}
	else {
		lua_pop(L, 1);
		return 0;
	}
	to->testString = testString;
	lua_pop(L, 1);

	lua_getfield(L, index, "testUserData");
	if (lua_isuserdata(L, 1)) {
		testUserData = lua_touserdata(L, 1);
	}
	else {
		lua_pop(L, 1);
		return 0;
	}
	to->testUserData = testUserData;
	lua_pop(L, 1);


	return 1;
}

int AB_toTest2(lua_State *L, int index, Test2 *to) {
	Test customParam;
	lua_getfield(L, index, "customParam");
	if (!AB_toTest(L, 1, &customParam)) {
		lua_pop(L, 1);
		return 0;
	}
	to->customParam = customParam;
	lua_pop(L, 1);


	return 1;
}


static AB_customTypeIn AB_customTypeIn_binding;
void AB_bind_customTypeIn(AB_customTypeIn function) {
	AB_customTypeIn_binding = function;
}


static int l_customTypeIn(lua_State *L) {
	Test customParam;
	const char *stringParam;


	if (lua_isstring(L, -1)) {
		stringParam = lua_tostring(L, -1);
	}
	else {
		return luaL_error(L, "Incorrect type for parameter stringParam. Expected type was string.");
	}

	if(lua_istable(L, -2)) {
		if(!AB_toTest(L, -2, &customParam)) {
			return luaL_Error(L, "Error loading custom type Test");
		}
	}
	else {
		return luaL_error(L, "Incorrect type for parameter customParam. Expected type was table.");
	}

	if (AB_customTypeIn_binding) {
		if (!AB_customTypeIn_binding(customParam, stringParam)) {
			return luaL_error(L, "Runtime error: customTypeIn failed for unknown reason!");
		}
	}
	else {
		return luaL_error(L, "Runtime error: customTypeIn wasn't bound!");
	}


	return 0;
}


void AB_registerModule_test(lua_State *L) {
	lua_register(L, "customTypeIn", l_customTypeIn);
}
