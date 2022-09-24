#include <lua.h>
typedef struct Test {
		double testNumber;
		char *testString;
		void *testUserData;
}Test;

typedef struct Test2 {
		Test customParam;
}Test2;

void AB_registerModule_test(lua_State *L);
typedef int (*AB_customTypeIn)(Test customParam, char *stringParam, char **outString, Test *customParamOut);
void AB_bind_customTypeIn(AB_customTypeIn function);
