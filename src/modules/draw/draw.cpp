#include "draw.hpp"
#include <glm/gtx/perpendicular.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "handle.hpp"

extern "C" {
#include "AB-Draw-interface.h"
#include <GLFW/glfw3.h>
}

static HandleBoundArray<glm::mat4> drawMatrices(10, [](){ return glm::mat4(1); });

static int DrawRectangle(double left, double top, double right, double bot) {
    glBegin(GL_TRIANGLES);
    glVertex2f(left, top);
    glVertex2f(left, bot);
    glVertex2f(right, top);

    glVertex2f(right, top);
    glVertex2f(left, bot);
    glVertex2f(right, bot);
    glEnd();

    return 1;
}

static int DrawLine(double xFrom, double yFrom, double xTo, double yTo, double width) {
    glm::vec2 from(xFrom, yFrom);
    glm::vec2 to(xTo, yTo);
    glm::vec2 lineVector = to - from;
    glm::vec2 normal(-lineVector.y, lineVector.x);
    normal = glm::normalize(normal);

    glm::vec2 vertices[6] = { 
        from    + (normal * (float)width * (float)0.5),
        from    - (normal * (float)width * (float)0.5),
        to      - (normal * (float)width * (float)0.5),
        from    + (normal * (float)width * (float)0.5),
        to      - (normal * (float)width * (float)0.5),
        to      + (normal * (float)width * (float)0.5)
    };

    glBegin(GL_TRIANGLES);
    for(const auto &v : vertices) {
        glVertex2f(v.x, v.y);
    }
    glEnd();

    return 1;
}

static int Clear(double red, double green, double blue, double alpha) {
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT);

    return 1;
}

static int SetDrawColor(double red, double green, double blue, double alpha) {
    glColor4f(red, green, blue, alpha);

    return 1;
}

static int CreateDrawMatrix(lua_Integer *handle) {
    uint32_t matrixHandle = drawMatrices.allocateHandle();
    *handle = (lua_Integer)(matrixHandle);
    return 1;
}

static int SetOrtho(lua_Integer handle, Rectangle bounds) {
    glm::mat4 *matrix = drawMatrices.get((uint32_t)handle);
    if(matrix == nullptr) {
        return 0;
    }
    *matrix = glm::ortho(bounds.left, bounds.right, bounds.bottom, bounds.top);
    return 1;
}

static int SetProjectionMatrix(lua_Integer matrixHandle) {
    glm::mat4 *matrix = drawMatrices.get((uint32_t)matrixHandle);
    if(matrix == nullptr) {
        return 0;
    }

    glMatrixMode(GL_PROJECTION_MATRIX);
    glLoadMatrixf(glm::value_ptr(*matrix));

    return 1;
}

static int SetViewport(Rectangle bounds) {
    glViewport(bounds.left, bounds.top, bounds.right, bounds.bottom);
    return 1;
}

void drawInitialize(lua_State *L) {
    AB_registerModule_Draw(L);
    AB_bind_DrawRectangle(DrawRectangle);
    AB_bind_Clear(Clear);
    AB_bind_SetDrawColor(SetDrawColor);
    AB_bind_DrawLine(DrawLine);
    AB_bind_CreateDrawMatrix(CreateDrawMatrix);
    AB_bind_SetOrtho(SetOrtho);
    AB_bind_SetProjectionMatrix(SetProjectionMatrix);
    AB_bind_SetViewport(SetViewport);
}