#include <lua.h>
void AB_registerModule_App(lua_State *L);
typedef int (*AB_CreateWindow)(double width, double height, const char *title, void **window);
typedef int (*AB_CloseWindow)(void *window);
typedef int (*AB_GetMouseXY)(void *window, double *X, double *Y);
typedef int (*AB_GetWindowDimensions)(void *window, double *width, double *height);
typedef int (*AB_SetTargetFPS)(double fps);
typedef int (*AB_GetSeconds)(double *seconds);

void AB_bind_CreateWindow(AB_CreateWindow function);
void AB_bind_CloseWindow(AB_CloseWindow function);
void AB_bind_GetMouseXY(AB_GetMouseXY function);
void AB_bind_GetWindowDimensions(AB_GetWindowDimensions function);
void AB_bind_SetTargetFPS(AB_SetTargetFPS function);
void AB_bind_GetSeconds(AB_GetSeconds function);

