#include <lua.h>
void AB_registerModule_test(lua_State *L);
typedef struct Test {
	double testNumber;
	const char *testString;
	void *testUserData;
}Test;

typedef struct Test2 {
	Test customParam;
}Test2;


typedef int (*AB_customTypeIn)(Test customParam);

void AB_bind_customTypeIn(AB_customTypeIn function);

