#include "draw.hpp"
#include <glm/gtx/perpendicular.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C" {
#include "AB-Draw-interface.h"
#include <GLFW/glfw3.h>
}

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

static int SetDrawCamera(Camera camera) {
    glm::mat4 projection = glm::ortho(
        camera.viewPort.left,
        camera.viewPort.right,
        camera.viewPort.bottom,
        camera.viewPort.top
    );

    glMatrixMode(GL_PROJECTION_MATRIX);
    glLoadMatrixf(glm::value_ptr(projection));
    glViewport(camera.viewPort.left, camera.viewPort.top, camera.viewPort.right, camera.viewPort.bottom);
    return 1;
}

void drawInitialize(lua_State *L) {
    /*
    Camera drawCamera;
    drawCamera.position = { 0, 0 };
    drawCamera.viewPort = { 0, 0, 1, 1 };
    SetDrawCamera(drawCamera);
    */
   
    AB_registerModule_Draw(L);
    AB_bind_DrawRectangle(DrawRectangle);
    AB_bind_Clear(Clear);
    AB_bind_SetDrawColor(SetDrawColor);
    AB_bind_DrawLine(DrawLine);
    AB_bind_SetDrawCamera(SetDrawCamera);
}