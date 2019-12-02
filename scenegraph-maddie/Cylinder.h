#ifndef CYLINDER_H
#define CYLINDER_H

#include <memory>
#include "GL/glew.h"
#include <QGLWidget>
#include "openglshape.h"
#include "gl/shaders/Shader.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "Shape.h"
#include "Cap.h"

/**
 * @class Cylinder
 *
 * @brief Generate the vertices and normals for a cylinder.
 */
class Cylinder : Shape {

public:
    Cylinder();
    ~Cylinder();

    void createVertices(int p1, int p2) override;
    void drawShape() override;
    // std::vector<float> createCap(int p1, int p2, int multiplier, glm::vec3 normal);
    std::vector<float> createBarrel(int p1, int p2, int multiplier);

    std::unique_ptr<Cap> TOP_CAP;
    std::unique_ptr<Cap> BOT_CAP;
    std::unique_ptr<OpenGLShape> BARREL;

};

#endif // CYLINDER_H
