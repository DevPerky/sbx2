#include <lua.h>
typedef struct Test {
		double testNumber;
		const char *testString;
		void *testUserData;
}Test;

typedef struct Test2 {
		Test customParam;
}Test2;

typedef struct TestIntegers {
		int integer;
}TestIntegers;

void AB_registerModule_test(lua_State *L);
typedef int (*AB_customTypeIn)(Test customParam, const char *stringParam, const char **outString, Test *customParamOut);
void AB_bind_customTypeIn(AB_customTypeIn function);
