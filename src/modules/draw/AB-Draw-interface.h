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
typedef int (*AB_DrawSetViewport)(Rectangle bounds);
typedef int (*AB_MatrixNew)(lua_Integer *handle);
typedef int (*AB_MatrixSetOrtho)(lua_Integer matrixHandle, Rectangle bounds);
typedef int (*AB_DrawCircle)(Vector2 position, double radius);
typedef int (*AB_DrawSetProjectionMatrix)(lua_Integer matrixHandle);
typedef int (*AB_DrawSetColor)(double red, double green, double blue, double alpha);
typedef int (*AB_DrawRectangle)(double left, double top, double right, double bottom);
typedef int (*AB_DrawLine)(double fromX, double fromY, double toX, double toY, double width);
typedef int (*AB_DrawClear)(double red, double green, double blue, double alpha);
void AB_bind_DrawSetViewport(AB_DrawSetViewport function);
void AB_bind_MatrixNew(AB_MatrixNew function);
void AB_bind_MatrixSetOrtho(AB_MatrixSetOrtho function);
void AB_bind_DrawCircle(AB_DrawCircle function);
void AB_bind_DrawSetProjectionMatrix(AB_DrawSetProjectionMatrix function);
void AB_bind_DrawSetColor(AB_DrawSetColor function);
void AB_bind_DrawRectangle(AB_DrawRectangle function);
void AB_bind_DrawLine(AB_DrawLine function);
void AB_bind_DrawClear(AB_DrawClear function);
#endif
