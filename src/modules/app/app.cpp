#include "app.hpp"
#include <thread>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern "C" {
#include <GLFW/glfw3.h>
#include "AB-App-interface.h"
}

static GLFWwindow *glfwWindow = NULL;
static double targetFPS = 60;

static int CreateWindow(double width, double height, const char *title, void **windowOut) {

	if(glfwWindow != NULL) {
		glfwDestroyWindow(glfwWindow);
		glfwWindow = NULL;
	}

	glfwWindow = glfwCreateWindow((int)width, (int)height, title, NULL, NULL);
	
	*windowOut = glfwWindow;

	return glfwWindow != 0;
}

static int CloseWindow(void *window) {
	GLFWwindow *win = (GLFWwindow*)window;

	if (win != glfwWindow) {
		return 0;
	}

	glfwSetWindowShouldClose(win, GLFW_TRUE);
	
	return 1;
}

static int GetMouseXY(void *window, double *x, double *y) {
	if((GLFWwindow*)window != glfwWindow) {
		return 0;
	}

	glfwGetCursorPos((GLFWwindow*)window, x, y);
	return 1;
}

static int SetTargetFPS(double fps) {
	if(fps < 0) {
		return 0;
	}

	targetFPS = fps;
	return 1;
}

static int GetSeconds(double *outSeconds) {
	if(glfwWindow == NULL) {
		return 0;
	}
	*outSeconds = glfwGetTime();

	return 1;
}

static int GetWindowDimensions(void *window, double *outWidth, double *outHeight) {
	if((GLFWwindow*)window != glfwWindow) {
		return 0;
	}

	int w, h;
	glfwGetWindowSize((GLFWwindow*)window, &w, &h);
	*outWidth = (double)w;
	*outHeight = (double)h;

	return 1;	
}

void appInitialize(lua_State *L) {
	AB_registerModule_App(L);

	AB_bind_CreateWindow(CreateWindow);
	AB_bind_CloseWindow(CloseWindow);
	AB_bind_GetMouseXY(GetMouseXY);
	AB_bind_SetTargetFPS(SetTargetFPS);
	AB_bind_GetSeconds(GetSeconds);
	AB_bind_GetWindowDimensions(GetWindowDimensions);
}

void appLoop(lua_State *L) {
    if (glfwWindow != NULL) {
		glfwMakeContextCurrent(glfwWindow);
		int winWidth, winHeight;
		glfwGetWindowSize(glfwWindow, &winWidth, &winHeight);
		auto projection = glm::ortho((float)0, (float)winWidth, (float)winHeight, (float)0);
		glMatrixMode(GL_PROJECTION_MATRIX);
		glLoadMatrixf(value_ptr(projection));

		while (!glfwWindowShouldClose(glfwWindow)) {
			auto frameStart = std::chrono::high_resolution_clock::now();
			auto frameEnd = frameStart + std::chrono::microseconds((int)(1000000 / targetFPS));

			glfwPollEvents();
			lua_getglobal(L, "update");
			lua_pcall(L, 0, 0, 0);

			glfwSwapBuffers(glfwWindow);
			std::this_thread::sleep_until(frameEnd);
		}
	}
}