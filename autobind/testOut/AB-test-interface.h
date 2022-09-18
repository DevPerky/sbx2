#include <lua.h>
void AB_registerModule_test(lua_State *L);
typedef int (*AB_threeInThreeOut)(double in_param1, const char *in_param2, void *in_param3, double *out_param, const char **out_param1, void **out_param2);
typedef int (*AB_theeInZeroOut)(double out_param, const char *out_param1, void *out_param2);
typedef int (*AB_zeroInThreeOut)(double *out_param, const char **out_param1, void **out_param2);

void AB_bind_threeInThreeOut(AB_threeInThreeOut function);
void AB_bind_theeInZeroOut(AB_theeInZeroOut function);
void AB_bind_zeroInThreeOut(AB_zeroInThreeOut function);

