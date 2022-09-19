#include "AB-test-interface.h"
#include <lauxlib.h>

static AB_customTypeIn AB_customTypeIn_binding;
void AB_bind_customTypeIn(AB_customTypeIn function) {
	AB_customTypeIn_binding = function;
}


static int l_customTypeIn(lua_State *L) {
	Test customParam;


	if (AB_customTypeIn_binding) {
		if (!AB_customTypeIn_binding(customParam)) {
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
