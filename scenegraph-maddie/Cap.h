#ifndef CAP_H
#define CAP_H

#include <memory>
#include "GL/glew.h"
#include <QGLWidget>
#include "openglshape.h"
#include "Shape.h"
#include "gl/shaders/Shader.h"

/**
 * @class Cap
 *
 * @brief Generate the vertices and normals for a circular cap.
 */
class Cap : Shape {

public:
    Cap(int multiplier, glm::vec3 normal);
    ~Cap();

    void createVertices(int p1, int p2) override;
    std::vector<float> buildCap(int p1, int p2);
    void drawShape() override;

private:
    std::unique_ptr<OpenGLShape> CAP;
    int m_multiplier;
    glm::vec3 m_normal;

};

#endif // CAP_H
