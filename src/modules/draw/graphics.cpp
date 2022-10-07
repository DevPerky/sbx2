#include "graphics.hpp"
#include <glad/glad.h>

bool Graphics::initialize(GraphicsInitProc initProc) {
    return gladLoadGLLoader(initProc);
}