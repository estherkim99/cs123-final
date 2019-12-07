#ifndef CUBE_H
#define CUBE_H

#include <memory>

#include "GL/glew.h"
#include <QGLWidget>
#include "lib/openglshape.h"
#include "gl/shaders/Shader.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "Shape.h"
/**
 * @class Cube
 *
 * @brief Generate the vertices and normals for a cube.
 */
class Cube : public Shape {

public:
    Cube();
    ~Cube();

    void createVertices(int p1, int p2) override;
    std::vector<float> createFace(int p1, glm::vec3 normal, glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 botLeft, glm::vec3 botRight);
    void drawShape() override;

    std::unique_ptr<OpenGLShape> POS_XY;
    std::unique_ptr<OpenGLShape> NEG_XY;
    std::unique_ptr<OpenGLShape> POS_XZ;
    std::unique_ptr<OpenGLShape> NEG_XZ;
    std::unique_ptr<OpenGLShape> POS_ZY;
    std::unique_ptr<OpenGLShape> NEG_ZY;

};

#endif // CUBE_H
