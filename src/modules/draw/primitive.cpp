#include "primitive.hpp"
#include <glm/ext/scalar_constants.hpp>
#include <math.h>

Primitive::Primitive(Type type, const std::vector<glm::vec2> &vertices) {
    m_vertices = vertices;
    m_type = type;
}

Primitive Primitive::createRectangle() {
    std::vector<glm::vec2> vertices {
        glm::vec2(-0.5, 0.5),
        glm::vec2(-0.5, -0.5),
        glm::vec2(0.5, -0.5),
        glm::vec2(0.5, 0.5)
    };

    return Primitive(Type::Circle, vertices);
}

Primitive Primitive::createTriangle() {
    std::vector<glm::vec2> vertices {
        glm::vec2(0.0, 0.5),
        glm::vec2(-0.5, -0.5),
        glm::vec2(0.5, 0.5)
    };

    return Primitive(Type::Triangle, vertices);
}

Primitive Primitive::createCircle(int resolution) {
    if(resolution < 3)  {
        resolution = 3;
    }

    switch(resolution % 3) {
        case 0:
            resolution = resolution;
        case 1:
            resolution-=1;
            break;
        case 2:
            resolution+=1;
            break;
    }

    std::vector<glm::vec2> vertices;
    vertices.push_back(glm::vec2(0, 0));

    for(int i = 0; i < resolution; i++) {
        constexpr float pi = glm::pi<float>();
        double radians = -(pi * 2 / (float)resolution) * (float)i;
        glm::vec2 vertexOnRadius(cos(radians), sin(radians));
        vertices.push_back(vertexOnRadius);
    }

    return Primitive(Type::Circle, vertices);
}



