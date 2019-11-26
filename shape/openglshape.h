#ifndef OPENGLSHAPE_H
#define OPENGLSHAPE_H

#include "GL/glew.h"

#include <memory>
#include <vector>

#include "gl/datatype/VBO.h"
#include "gl/datatype/VBOAttribMarker.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"

namespace CS123 { namespace GL {
class VAO;
}}

using namespace CS123::GL;


class OpenGLShape
{
public:
    OpenGLShape();

    virtual ~OpenGLShape();

    /** Initialize the VBO with the given vertex data. */
    void setVertexData(GLfloat *data, int size, VBO::GEOMETRY_LAYOUT drawMode, int numVertices);

    /** Enables the specified attribute and calls glVertexAttribPointer with the given arguments. */
    void setAttribute(GLuint index, GLuint numElementsPerVertex, int offset, VBOAttribMarker::DATA_TYPE type,
                      bool normalize);

    /** Build the VAO given the specified vertex data and atrributes */
    void buildVAO();

    /** Draw the initialized geometry. */
    void draw();

    /** Initialize shapes with correct vertex data. */
    virtual void initializeShape(int p1, int p2);

 protected:
    /** Rotates a surface described by vertex coordinates and normals about input dir axis, by input angle. */
    void rotateSurface(std::vector<float>* target, glm::vec3 dir, float angle, int dataNum);

 private:
    /** Set vertex data, by calling setVertexData for vertex coordinates and normals */
    void setData(std::vector<float>* vertices);

    /** Defines coordinates for side of a shape */
    virtual void makeSides(std::vector<float>* vertices, int p1, int p2) = 0;

    GLfloat *m_data;                            /// vector of floats containing the vertex data.
    GLsizeiptr m_size;                          /// size of the data array, in bytes.
    VBO::GEOMETRY_LAYOUT m_drawMode;            /// drawing mode
    int m_numVertices;                          /// number of vertices to be rendered
    std::vector<VBOAttribMarker> m_markers;     /// list of VBOAttribMarkers that describe how the data is laid out.
    std::unique_ptr<CS123::GL::VAO> m_VAO;      /// a wrapper for the vertex array object (VAO)

};

#endif // OPENGLSHAPE_H
