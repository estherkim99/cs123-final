#ifndef SPHERE_H
#define SPHERE_H
#include <memory>
#include "GL/glew.h"
#include <QGLWidget>
#include "openglshape.h"
#include "gl/shaders/Shader.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "Sphere.h"
#include "Shape.h"

/**
 * @class Sphere
 *
 * @brief Generate the vertices and normals for a sphere.
 */
class Sphere : Shape {

public:
    Sphere();
    ~Sphere();

    void createVertices(int p1, int p2) override;
    void drawShape() override;
    std::vector<float> createBody(int p1, int p2);

    std::unique_ptr<OpenGLShape> BODY;

};

#endif // SPHERE_H
