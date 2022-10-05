#ifndef AB_DRAW_H
#define AB_DRAW_H

#include <lua.h>

typedef struct Vector2 {
	double x;
	double y;
}Vector2;

typedef struct Rectangle {
	double left;
	double top;
	double right;
	double bottom;
}Rectangle;

typedef struct Camera {
	Vector2 position;
	Rectangle viewPort;
}Camera;

void AB_registerModule_Draw(lua_State *L);
typedef int (*AB_SetViewport)(Rectangle bounds);
typedef int (*AB_CreateDrawMatrix)(lua_Integer *handle);
typedef int (*AB_SetOrtho)(lua_Integer matrixHandle, Rectangle bounds);
typedef int (*AB_SetProjectionMatrix)(lua_Integer matrixHandle);
typedef int (*AB_SetDrawColor)(double red, double green, double blue, double alpha);
typedef int (*AB_DrawRectangle)(double left, double top, double right, double bottom);
typedef int (*AB_DrawLine)(double fromX, double fromY, double toX, double toY, double width);
typedef int (*AB_Clear)(double red, double green, double blue, double alpha);
void AB_bind_SetViewport(AB_SetViewport function);
void AB_bind_CreateDrawMatrix(AB_CreateDrawMatrix function);
void AB_bind_SetOrtho(AB_SetOrtho function);
void AB_bind_SetProjectionMatrix(AB_SetProjectionMatrix function);
void AB_bind_SetDrawColor(AB_SetDrawColor function);
void AB_bind_DrawRectangle(AB_DrawRectangle function);
void AB_bind_DrawLine(AB_DrawLine function);
void AB_bind_Clear(AB_Clear function);
#endif
