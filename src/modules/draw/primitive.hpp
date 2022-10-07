#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include <glm/vec2.hpp>
#include <vector>

class Primitive {
public:
    enum Type {
        Rectangle,
        Triangle,
        Circle
    };

private:
    std::vector<glm::vec2> m_vertices;
    Type m_type;
    Primitive(Type type, const std::vector<glm::vec2> &vertices);

public:
    inline const std::vector<glm::vec2> getVertices() const {
        return m_vertices;
    }
    
    static Primitive CreateRectangle();
    static Primitive CreateTriangle();
    static Primitive CreateCircle(int resolution);
};

#endif