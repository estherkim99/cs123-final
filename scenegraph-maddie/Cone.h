#ifndef CONE_H
#define CONE_H

#include <memory>
#include "GL/glew.h"
#include <QGLWidget>
#include "openglshape.h"
#include "gl/shaders/Shader.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "Cylinder.h"
#include "Shape.h"

/**
 * @class Cone
 *
 * @brief Generate the vertices and normals for a cone.
 */
class Cone : Shape {

public:
    Cone();
    ~Cone();

    void createVertices(int p1, int p2) override;
    void drawShape() override;
    std::vector<float> createBody(int p1, int p2);
    glm::vec3 crossProduct(glm::vec3 x, glm::vec3 y);
    glm::vec3 normalize(glm::vec3 x);

    std::unique_ptr<Cap> CAP;
    std::unique_ptr<OpenGLShape> BODY;

};

#endif // CONE_H
