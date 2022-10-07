#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef void *(*GraphicsInitProc)(const char *name);
class Graphics {
public:
    static bool initialize(GraphicsInitProc initProc);
};

#endif
