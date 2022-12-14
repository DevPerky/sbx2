#ifndef AB_TEST_H
#define AB_TEST_H

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
	lua_Integer integer;
}TestIntegers;

void AB_registerModule_test(lua_State *L);
typedef int (*AB_testIntegers)(lua_Integer integerIn, lua_Integer *integerOut);
typedef int (*AB_customTypeIn)(Test customParam, const char *stringParam, const char **outString, Test *customParamOut);
void AB_bind_testIntegers(AB_testIntegers function);
void AB_bind_customTypeIn(AB_customTypeIn function);
#endif
